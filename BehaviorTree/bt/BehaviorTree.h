#pragma once

#include "Blackboard.h"

#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace bt
{
enum class Status { Success, Failure, Running };
// One data-changing action per actor per simulation tick. Conditions are free.
struct Frame { bool actionUsed = false; };

class Node
{
public:
    virtual ~Node() = default;
    virtual Status Tick(Frame& frame) = 0;
    virtual void Reset() = 0;
};
using NodePtr = std::unique_ptr<Node>;

class Action final : public Node
{
public:
    Action(std::function<Status()> execute, std::function<void()> enter = {},
        std::function<void()> exit = {})
        : execute_(std::move(execute)), enter_(std::move(enter)), exit_(std::move(exit)) {}

    Status Tick(Frame& frame) override
    {
        if (!active_)
        {
            active_ = true;
            if (enter_) enter_();
        }
        if (frame.actionUsed) return Status::Running;
        frame.actionUsed = true;
        const auto result = execute_();
        if (result != Status::Running) Reset();
        return result;
    }
    void Reset() override
    {
        if (active_ && exit_) exit_();
        active_ = false;
    }
private:
    std::function<Status()> execute_;
    std::function<void()> enter_;
    std::function<void()> exit_;
    bool active_ = false;
};

class Condition final : public Node
{
public:
    explicit Condition(std::function<bool()> predicate) : predicate_(std::move(predicate)) {}
    Status Tick(Frame&) override { return predicate_() ? Status::Success : Status::Failure; }
    void Reset() override {}
private:
    std::function<bool()> predicate_;
};

// Memory composites: resume the Running child on the next tick.
class Sequence final : public Node
{
public:
    explicit Sequence(std::vector<NodePtr> children) : children_(std::move(children)) {}
    Status Tick(Frame& frame) override
    {
        while (index_ < children_.size())
        {
            const auto result = children_[index_]->Tick(frame);
            if (result == Status::Running) return result;
            if (result == Status::Failure) { Reset(); return result; }
            ++index_;
        }
        Reset();
        return Status::Success;
    }
    void Reset() override
    {
        for (auto& child : children_) child->Reset();
        index_ = 0;
    }
private:
    std::vector<NodePtr> children_;
    std::size_t index_ = 0;
};

class Selector final : public Node
{
public:
    explicit Selector(std::vector<NodePtr> children) : children_(std::move(children)) {}
    Status Tick(Frame& frame) override
    {
        while (index_ < children_.size())
        {
            const auto result = children_[index_]->Tick(frame);
            if (result == Status::Running) return result;
            if (result == Status::Success) { Reset(); return result; }
            ++index_;
        }
        Reset();
        return Status::Failure;
    }
    void Reset() override
    {
        for (auto& child : children_) child->Reset();
        index_ = 0;
    }
private:
    std::vector<NodePtr> children_;
    std::size_t index_ = 0;
};

class Repeat final : public Node
{
public:
    explicit Repeat(NodePtr child) : child_(std::move(child)) {}
    Status Tick(Frame& frame) override
    {
        const auto result = child_->Tick(frame);
        if (result == Status::Failure) return result;
        if (result == Status::Success)
        {
            child_->Reset();
            // Enter the next cycle now, but never execute a second action.
            Frame enterOnly{true};
            (void)child_->Tick(enterOnly);
        }
        return Status::Running;
    }
    void Reset() override { child_->Reset(); }
private:
    NodePtr child_;
};

// Reactive high-priority branch. The fallback may be aborted or suspended.
class PrioritySelector final : public Node
{
public:
    PrioritySelector(std::function<bool()> urgent, NodePtr high, NodePtr fallback,
        bool preserveFallback)
        : urgent_(std::move(urgent)), high_(std::move(high)), fallback_(std::move(fallback)),
          preserveFallback_(preserveFallback) {}
    Status Tick(Frame& frame) override
    {
        if (urgent_())
        {
            if (!usingHigh_ && !preserveFallback_) fallback_->Reset();
            usingHigh_ = true;
            return high_->Tick(frame);
        }
        if (usingHigh_) high_->Reset();
        usingHigh_ = false;
        return fallback_->Tick(frame);
    }
    void Reset() override
    {
        high_->Reset();
        fallback_->Reset();
        usingHigh_ = false;
    }
private:
    std::function<bool()> urgent_;
    NodePtr high_;
    NodePtr fallback_;
    bool preserveFallback_;
    bool usingHigh_ = false;
};

template <typename... Children>
std::vector<NodePtr> ChildrenOf(Children&&... children)
{
    std::vector<NodePtr> result;
    (result.push_back(std::forward<Children>(children)), ...);
    return result;
}

class Model
{
public:
    [[nodiscard]] Blackboard& Local() noexcept { return local_; }
    [[nodiscard]] const Blackboard& Local() const noexcept { return local_; }
    [[nodiscard]] Blackboard& Shared() noexcept { return shared_; }
    [[nodiscard]] const Blackboard& Shared() const noexcept { return shared_; }
    virtual ~Model() = default;
    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;
    Model(Model&&) = delete;
    Model& operator=(Model&&) = delete;
    [[nodiscard]] virtual Status Update()
    {
        Frame frame;
        return root_->Tick(frame);
    }
protected:
    explicit Model(Blackboard& shared) noexcept : shared_(shared) {}
    void SetTree(NodePtr root)
    {
        root_ = std::move(root);
        Frame enterOnly{true};
        (void)root_->Tick(enterOnly);
    }
private:
    Blackboard local_;
    Blackboard& shared_;
    NodePtr root_;
};
}
