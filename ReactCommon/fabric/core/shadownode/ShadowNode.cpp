/**
 * Copyright (c) 2015-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ShadowNode.h"

#include <fabric/debug/DebugStringConvertible.h>
#include <fabric/debug/DebugStringConvertibleItem.h>

namespace facebook {
namespace react {

SharedShadowNodeSharedList ShadowNode::emptySharedShadowNodeSharedList() {
  static const auto emptySharedShadowNodeSharedList = std::make_shared<SharedShadowNodeList>();
  return emptySharedShadowNodeSharedList;
}

#pragma mark - Constructors

ShadowNode::ShadowNode(
  Tag tag,
  Tag rootTag,
  InstanceHandle instanceHandle,
  SharedProps props,
  SharedShadowNodeSharedList children
):
  tag_(tag),
  rootTag_(rootTag),
  instanceHandle_(instanceHandle),
  props_(props),
  children_(std::make_shared<SharedShadowNodeList>(*children)),
  revision_(1) {}

ShadowNode::ShadowNode(
  SharedShadowNode shadowNode,
  SharedProps props,
  SharedShadowNodeSharedList children
):
  tag_(shadowNode->tag_),
  rootTag_(shadowNode->rootTag_),
  instanceHandle_(shadowNode->instanceHandle_),
  props_(props ? props : shadowNode->props_),
  children_(std::make_shared<SharedShadowNodeList>(*(children ? children : shadowNode->children_))),
  sourceNode_(shadowNode),
  revision_(shadowNode->revision_ + 1) {}

#pragma mark - Getters

SharedShadowNodeSharedList ShadowNode::getChildren() const {
  return children_;
}

SharedProps ShadowNode::getProps() const {
  return props_;
}

Tag ShadowNode::getTag() const {
  return tag_;
}

Tag ShadowNode::getRootTag() const {
  return rootTag_;
}

InstanceHandle ShadowNode::getInstanceHandle() const {
  return instanceHandle_;
}

SharedShadowNode ShadowNode::getSourceNode() const {
  return sourceNode_.lock();
}

void ShadowNode::sealRecursive() const {
  if (getSealed()) {
    return;
  }

  seal();

  props_->seal();

  for (auto child : *children_) {
    child->sealRecursive();
  }
}

#pragma mark - Mutating Methods

void ShadowNode::appendChild(const SharedShadowNode &child) {
  ensureUnsealed();

  auto nonConstChildren = std::const_pointer_cast<SharedShadowNodeList>(children_);
  nonConstChildren->push_back(child);
}

void ShadowNode::replaceChild(const SharedShadowNode &oldChild, const SharedShadowNode &newChild) {
  ensureUnsealed();

  auto nonConstChildren = std::const_pointer_cast<SharedShadowNodeList>(children_);
  std::replace(nonConstChildren->begin(), nonConstChildren->end(), oldChild, newChild);
}

void ShadowNode::clearSourceNode() {
  ensureUnsealed();
  sourceNode_.reset();
}

void ShadowNode::shallowSourceNode() {
  ensureUnsealed();

  auto sourceNode = sourceNode_.lock();
  assert(sourceNode);
  sourceNode_ = sourceNode->getSourceNode();
}

#pragma mark - Equality

bool ShadowNode::operator==(const ShadowNode& rhs) const {
  // Note: Child nodes are not considered as part of instance's value
  // and/or identity.
  return
    tag_ == rhs.tag_ &&
    rootTag_ == rhs.rootTag_ &&
    props_ == rhs.props_;
}

bool ShadowNode::operator!=(const ShadowNode& rhs) const {
  return !(*this == rhs);
}

#pragma mark - DebugStringConvertible

std::string ShadowNode::getDebugName() const {
  return getComponentName();
}

std::string ShadowNode::getDebugValue() const {
  return "r" + std::to_string(revision_) + (getSealed() ? "/sealed" : "");
}

SharedDebugStringConvertibleList ShadowNode::getDebugChildren() const {
  SharedDebugStringConvertibleList debugChildren = {};

  for (auto child : *children_) {
    auto debugChild = std::dynamic_pointer_cast<const DebugStringConvertible>(child);
    if (debugChild) {
      debugChildren.push_back(debugChild);
    }
  }

  return debugChildren;
}

SharedDebugStringConvertibleList ShadowNode::getDebugProps() const {
  SharedDebugStringConvertibleList list = {};

  list.push_back(std::make_shared<DebugStringConvertibleItem>("tag", std::to_string(tag_)));

  if (instanceHandle_) {
    list.push_back(std::make_shared<DebugStringConvertibleItem>("handle", std::to_string((size_t)instanceHandle_)));
  }

  SharedShadowNode sourceNode = getSourceNode();
  if (sourceNode) {
    list.push_back(std::make_shared<DebugStringConvertibleItem>(
      "source",
      sourceNode->getDebugDescription({.maximumDepth = 1, .format = false})
    ));
  }

  SharedDebugStringConvertibleList propsList = props_->getDebugProps();
  std::move(propsList.begin(), propsList.end(), std::back_inserter(list));
  return list;
}

} // namespace react
} // namespace facebook
