"""Render code-based diagrams only; does not build/run either simulator."""
from pathlib import Path
from html import escape
import math
from PIL import Image, ImageDraw, ImageFont

OUT = Path(__file__).parent
FONT = 'C:/Windows/Fonts/malgun.ttf'
BOLD = 'C:/Windows/Fonts/malgunbd.ttf'
INK = '#172B46'
MUTED = '#566982'
BLUE = '#2765B0'
GREEN = '#167769'
ORANGE = '#B65C20'
PURPLE = '#7251A3'

class Canvas:
    def __init__(self, height, title, subtitle):
        self.h = height
        self.im = Image.new('RGB', (1800, height), '#F3F6FA')
        self.d = ImageDraw.Draw(self.im)
        self.svg = [f'<svg xmlns="http://www.w3.org/2000/svg" width="1800" height="{height}" viewBox="0 0 1800 {height}">',
                    f'<rect width="1800" height="{height}" fill="#F3F6FA"/>']
        self.text(50, 35, title, 38, bold=True)
        self.text(50, 95, subtitle, 22, MUTED)

    def text(self, x, y, value, size=22, color=INK, bold=False):
        for i, line in enumerate(value.split('\n')):
            yy = y + i * (size + 9)
            font = ImageFont.truetype(BOLD if bold else FONT, size)
            self.d.text((x, yy), line, font=font, fill=color)
            self.svg.append(f'<text x="{x}" y="{yy+size}" font-family="Malgun Gothic, sans-serif" font-size="{size}" font-weight="{700 if bold else 400}" fill="{color}">{escape(line)}</text>')

    def rect(self, x, y, w, h, fill='#FFFFFF', border='#DAE2EC', radius=14):
        self.d.rounded_rectangle((x,y,x+w,y+h),radius,fill=fill,outline=border,width=2)
        self.svg.append(f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="{radius}" fill="{fill}" stroke="{border}" stroke-width="2"/>')

    def line(self, points, color=MUTED, arrow=False, dashed=False):
        for (x1,y1),(x2,y2) in zip(points,points[1:]):
            if dashed:
                length=math.hypot(x2-x1,y2-y1)
                for start in range(0,int(length),16):
                    a=start/max(length,1); b=min(start+9,length)/max(length,1)
                    self.d.line((x1+(x2-x1)*a,y1+(y2-y1)*a,x1+(x2-x1)*b,y1+(y2-y1)*b),fill=color,width=3)
            else:
                self.d.line((x1,y1,x2,y2),fill=color,width=3)
        pts=' '.join(f'{x},{y}' for x,y in points)
        dash=' stroke-dasharray="9 7"' if dashed else ''
        self.svg.append(f'<polyline points="{pts}" fill="none" stroke="{color}" stroke-width="3"{dash}/>')
        if arrow:
            x,y=points[-1]; a,b=points[-2]; angle=math.atan2(y-b,x-a)
            triangle=[(x,y),(x-13*math.cos(angle-.45),y-13*math.sin(angle-.45)),(x-13*math.cos(angle+.45),y-13*math.sin(angle+.45))]
            self.d.polygon(triangle,fill=color)
            self.svg.append(f'<polygon points="{" ".join(f"{a},{b}" for a,b in triangle)}" fill="{color}"/>')

    def box(self,x,y,w,title,body='',color=BLUE,h=94):
        self.rect(x,y,w,h,border=color)
        self.text(x+15,y+10,title,24,color,True)
        if body: self.text(x+15,y+47,body,18)

    def panel(self,y,h,title,color):
        self.rect(35,y,1730,h)
        self.text(60,y+18,title,27,color,True)

    def save(self,name):
        self.text(50,self.h-45,'현재 소스 기준 · 2026-10-11 · 실행 결과가 아닌 구조/호출 설명',18,MUTED)
        self.im.save(OUT/f'{name}.png')
        (OUT/f'{name}.svg').write_text('\n'.join(self.svg+['</svg>']),encoding='utf-8')

def states():
    c=Canvas(1690,'01  FSM 상태도','실선: 상태 전환 · 점선: 공통 개입/복귀 · 화장실은 모든 일반 상태보다 우선')
    c.panel(150,350,'나무꾼  |  작업 → 판매 → 주점 → 휴식',BLUE)
    titles=['출근','벌목','판매','주점 방문','음주','취침']
    bodies=['Go_To_Work\n거리 +1 (목표 5)','WoodCutting\n체력 -1 / 나무 +1','Go_To_Sell\n나무 -1 / 코인 +5','VisitTavern\nBeginVisit 호출','Drinking\n코인 -5 / 취기 +5','Sleeping\n5회 행동']
    for i,(t,b) in enumerate(zip(titles,bodies)):
        c.box(75+i*285,245,240,t,b,h=115)
    for i,label in enumerate(['거리 ≥ 5','체력 = 0','나무 = 0','1회 행동','취기 ≥ 20']):
        x=315+i*285
        c.line([(x,280),(x+45,280)],BLUE,True)
        c.text(x-25,210,label,17,BLUE)
    c.line([(1620,360),(1620,408),(195,408),(195,360)],BLUE,True)
    c.text(465,415,'취침 Exit: 취기 0 / 체력 10 / 거리 0 → 다시 출근',21,BLUE)
    c.text(75,465,'판매: 구매 불가 시 대기  |  음주: 돈 부족 또는 술 제공 거절 시 취침  |  출근·벌목·판매 완료 판정은 다음 틱',18,MUTED)

    c.panel(530,630,'상점주인  |  영업·휴식 + 외부 거래 + Global State',GREEN)
    c.box(80,625,240,'개점','Open_for_business',GREEN)
    c.box(440,625,250,'영업','Working\n졸림·흡연 욕구 +1',GREEN,115)
    c.box(850,595,285,'수면','Sleep / 졸림 -1',GREEN)
    c.box(850,790,285,'흡연','Cigarette / 욕구 -1',GREEN)
    c.box(440,935,250,'구매','Buying',GREEN)
    c.line([(320,675),(440,675)],GREEN,True);c.text(334,640,'1행동',18,GREEN)
    c.line([(690,650),(770,650),(770,625),(850,625)],GREEN,True);c.text(702,595,'sleep > 10',18,GREEN)
    c.line([(850,670),(795,670),(795,705),(690,705)],GREEN,True);c.text(706,717,'sleep = 0',18,GREEN)
    c.line([(600,740),(600,820),(850,820)],GREEN,True);c.text(615,781,'cigarette > 5',18,GREEN)
    c.line([(850,858),(755,858),(755,757),(670,757),(670,740)],GREEN,True);c.text(825,895,'cigarette = 0',18,GREEN)
    c.line([(470,935),(470,740)],GREEN,True);c.text(280,840,'EndBuying()',18,GREEN)
    c.box(80,935,270,'외부 거래 요청','BeginBuying()\n화장실 외 → 구매',GREEN,115)
    c.line([(350,980),(440,980)],GREEN,True)
    c.text(80,1080,'영업에서 휴식 조건이 겹치면 수면 우선. 거래가 끝나면 영업으로 복귀.',20,MUTED)

    c.box(1240,605,480,'GlobalState.Execute','화장실 외 매 틱 toilet +1\ntoilet ≥ 10 → 화장실로 전환',ORANGE,120)
    c.line([(1480,725),(1480,785)],ORANGE,True,True)
    c.text(1250,745,'개점·영업·구매·수면·흡연 모두 적용',18,ORANGE)
    c.box(1240,785,480,'화장실 / Toilet','5회 행동 → toilet = 0',ORANGE)
    c.line([(1480,879),(1480,925)],ORANGE,True,True)
    c.box(1240,925,480,'복귀 판단','거래 변경 없음 → 이전 상태\n거래 변경 있음 → 구매 요청 ? 구매 : 영업',ORANGE,120)
    c.text(1240,1070,'화장실 중 재진입 없음 / 외부 거래 요청 보류\n흡연 복귀 시 남은 진행도 유지',18,MUTED)

    c.panel(1190,420,'주점 주인  |  작업 중단 → 손님 접대 → 이전 작업 재개',PURPLE)
    c.box(100,1310,350,'설거지','WashingDishes / 5회',PURPLE)
    c.box(650,1310,350,'주점 청소','Cleaning / 10회',PURPLE)
    c.box(1280,1310,400,'손님 접대','ServingCustomer\n방문·음주 상태에 따라 대화',PURPLE,115)
    c.line([(450,1335),(650,1335)],PURPLE,True);c.text(475,1295,'설거지 완료',18,PURPLE)
    c.line([(650,1380),(450,1380)],PURPLE,True);c.text(490,1400,'청소 완료',18,PURPLE)
    c.line([(275,1310),(275,1275),(1480,1275),(1480,1310)],PURPLE,True,True)
    c.line([(825,1310),(825,1275)],PURPLE,dashed=True)
    c.text(600,1242,'BeginVisit → 작업 진행도를 보존하고 접대',18,PURPLE)
    c.line([(1480,1425),(1480,1500),(275,1500),(275,1404)],PURPLE,True,True)
    c.line([(825,1500),(825,1404)],PURPLE,True,True)
    c.text(520,1510,'EndVisit → 직전 작업 하나로 복귀 (진행도 유지)',20,PURPLE)
    c.text(80,1560,'주점의 화살표 두 갈래는 동시 복귀가 아니라, 중단 전에 실행 중이던 작업으로의 선택적 복귀다.',18,MUTED)
    c.save('01_fsm_states')

def messages():
    c=Canvas(1810,'02  FSM 메시지·호출 흐름도','현재 구현은 메시지 큐/Dispatcher가 아닌 동기식 직접 함수 호출이다. 시간은 위에서 아래로 흐른다.')
    xs=[240,850,1460]
    for x,title,color in zip(xs,['나무꾼','상점주인','주점 주인'],[BLUE,GREEN,PURPLE]):
        c.box(x-140,170,280,title,color=color,h=65)
        c.line([(x,235),(x,1690)],color,dashed=True)
    def msg(a,b,y,label,color=BLUE,ret=False):
        c.line([(xs[a],y),(xs[b],y)],color,True,ret)
        c.text(min(xs[a],xs[b])+25,y-33,label,21,color)
    def note(x,y,w,txt,color=MUTED,h=64):
        c.rect(x,y,w,h,'#F8FAFD',color)
        c.text(x+12,y+10,txt,19,color)
    msg(0,1,295,'판매 Enter → BeginBuying()')
    note(890,315,790,'buyingRequested = true / 일반 상태면 Buying\n화장실이면 요청만 기록하고 현재 행동 유지',GREEN,78)
    msg(0,1,445,'판매 Execute → CanBuyWood()')
    msg(1,0,505,'반환: Buying 상태인가? (bool)',GREEN,True)
    note(65,535,540,'true: 나무 -1 / 코인 +5\nfalse: 자원 변경 없이 다음 틱까지 대기',BLUE,82)
    note(920,540,760,'상점주인 Update (매 틱)\nGlobal.Execute → toilet ≥ 10이면 Toilet\n현재 상태 Execute → 화장실 5행동 후 복귀',ORANGE,112)
    c.text(310,655,'위 구매 가능 확인과 판매를 나무가 남아 있는 동안 반복',20,MUTED)
    msg(0,1,730,'판매 Exit → EndBuying()')
    note(895,750,785,'buyingRequested = false / 일반 상태면 Working\n화장실이면 복귀 시 영업으로 돌아가도록 기록',GREEN,80)
    msg(0,2,895,'주점 방문 Enter → BeginVisit(나무꾼)')
    note(1050,920,650,'customer 등록 / ServingCustomer로 전환\n직전 설거지·청소와 진행도를 보존',PURPLE,80)
    msg(0,2,1070,'음주 Execute → ServeDrink(나무꾼)')
    note(1045,1095,660,'손님 일치 + 접대 중 + 음주 상태 + 코인 ≥ 5\n조건 만족 시 술 판매 대사 출력',PURPLE,80)
    msg(2,0,1230,'반환: 술 제공 가능 여부 (bool)',PURPLE,True)
    note(65,1260,820,'성공: 코인 -5 / 취기 +5 / 나무꾼 답변 출력\n취기 20이면 취침. 돈 부족·제공 실패도 취침으로 전환.',BLUE,82)
    c.text(950,1300,'정상 방문: 음주 4회 반복',21,MUTED)
    msg(0,2,1410,'음주 Exit → EndVisit(나무꾼)')
    note(1060,1435,635,'customer 해제 / RevertToPreviousState()\n접대 전 작업과 진행도를 이어감',PURPLE,80)
    note(65,1550,770,'취침 5행동 → Exit에서 취기 0 / 체력 10 / 거리 0\n출근 상태로 돌아가 전체 시나리오 반복',BLUE,82)
    c.rect(50,1695,1700,60,'#E8EEF6')
    c.text(75,1710,'매 틱 갱신 순서: 나무꾼 → 상점주인 → 주점 주인  |  실선: 호출 / 점선 반환 화살표: 결과 반환',20)
    c.save('02_fsm_message_flow')

def trees():
    c=Canvas(2010,'03  Behavior Tree + Blackboard','트리 구조는 실제 생성자 구성 기준. 인물별 Update는 한 틱에 최대 한 개의 Action 본문을 실행한다.')
    c.rect(40,155,1720,76,'#E8EEF6')
    c.text(65,169,'REP 반복   ·   SEQ 순차 실행   ·   SEL 후보 선택   ·   PRI 우선 조건 재검사   ·   ? 조건   ·   A 행동',22)
    c.text(65,201,'Running: 계속 실행 / Success: 완료 / Failure: 다음 후보 또는 실패 처리',17,MUTED)
    def tree_panel(x,w,title,nodes,color):
        c.rect(x,260,w,1250)
        c.text(x+20,280,title,27,color,True)
        stack={}
        for i,(depth,title,body) in enumerate(nodes):
            y=350+i*88
            nx=x+20+depth*28
            if depth:
                py=stack[depth-1]
                c.line([(nx-15,py+54),(nx-15,y+26),(nx,y+26)],'#A9B8CA')
            c.rect(nx,y,w-40-depth*28,64,'#FFFFFF',color,8)
            c.text(nx+10,y+5,title,20,color,True)
            if body: c.text(nx+10,y+33,body,16,MUTED)
            stack[depth]=y
    tree_panel(40,510,'나무꾼',[
        (0,'REP  하루 행동 반복','완료하면 다시 출근'),
        (1,'SEQ  순서대로 실행','Running 자식 위치 기억'),
        (2,'A  출근','거리 5 → 다음 틱 완료'),
        (2,'A  벌목','체력 -1 / 나무 +1'),
        (2,'A  판매','화장실이면 대기 / 코인 +5'),
        (2,'A  주점 방문','방문 요청 설정 / 1행동'),
        (2,'A  음주','코인 -5 / 취기 +5 → 20'),
        (2,'A  취침','5행동 → 취기 0 / 체력 10')],BLUE)
    c.text(65,1120,'판매·방문·음주 요청을 Shared에 기록\n각 수치와 진행도는 Local에서 읽고 갱신\n음주 행동에서 주점 주인 ServeDrink 호출',19,MUTED)
    tree_panel(570,660,'상점주인',[
        (0,'PRI  Toilet ≥ 10 ?','최상위: 정상 트리 일시 중단·보존'),
        (1,'A  화장실 [조건 참]','5행동 / Toilet=0 / 이용 여부 해제'),
        (1,'SEQ  정상 행동 [조건 거짓]','중단된 위치에서 재개'),
        (2,'A  개점','처음 1회'),
        (2,'PRI  SellingWood ?','거래 진입 시 평상시 트리 Reset'),
        (3,'A  구매 [조건 참]','판매 요청 동안 Running'),
        (3,'REP  평상시 [조건 거짓]','거래 종료 후 영업부터 재시작'),
        (4,'SEQ  영업 → 휴식',''),
        (5,'A  영업','졸림·흡연 욕구 +1 / 휴식 필요 시 완료'),
        (5,'SEL  수면 우선',''),
        (6,'SEQ  ? Sleep > 10 → A 수면','수치 1씩 감소 / 0인 다음 행동 완료'),
        (6,'SEQ  ? Cigarette > 5 → A 흡연','시작 시 5 / 수치 1씩 감소')],GREEN)
    c.text(592,1430,'Update 전처리: 화장실 외 매 틱 Toilet +1',19,ORANGE)
    tree_panel(1250,510,'주점 주인',[
        (0,'PRI  VisitingTavern ?','작업 트리는 Reset하지 않고 보존'),
        (1,'A  손님 접대 [조건 참]','Drinking 여부에 따라 대화'),
        (1,'REP  작업 [조건 거짓]','손님 퇴장 시 이전 작업 재개'),
        (2,'SEQ  설거지 → 청소',''),
        (3,'A  설거지','5행동 → 진행도 0'),
        (3,'A  주점 청소','10행동 → 진행도 0')],PURPLE)
    c.text(1275,990,'접대 동안 작업 위치·진행도 보존\n퇴장 후 남은 작업부터 이어서 실행',19,MUTED)
    c.rect(40,1540,1720,380,'#FFFFFF')
    c.text(65,1560,'BLACKBOARD  |  데이터 저장과 인물 간 공유',27,PURPLE,True)
    c.box(65,1620,1670,'Shared() — 세 인물이 같은 인스턴스를 참조',
        '나무꾼 기록 → SellingWood / VisitingTavern / Drinking     |     상점주인 기록 → SellerInToilet',PURPLE,90)
    c.box(65,1740,510,'Local() — 나무꾼','Wood / WorkLength / Stamina / Coin\nIntoxication / SleepProgress',BLUE,115)
    c.box(610,1740,535,'Local() — 상점주인','Sleep / Cigarette\nToilet / ToiletProgress',GREEN,115)
    c.box(1180,1740,555,'Local() — 주점 주인','DishProgress / CleaningProgress',PURPLE,115)
    c.text(65,1875,'모든 행동·조건은 타입 키로 Get/Set. 트리 실행 위치는 노드가 기억하며, 블랙보드는 값을 저장한다.',20,MUTED)
    c.save('03_behavior_tree')

if __name__ == '__main__':
    states(); messages(); trees()
    print('Created 3 PNG diagrams and 3 editable SVG diagrams in', OUT)
