/**
 * Copyright (c) 2015 - The CM Authors <legal@clickmatcher.com>
 *   All Rights Reserved.
 *
 * This file is CONFIDENTIAL -- Distribution or duplication of this material or
 * the information contained herein is strictly forbidden unless prior written
 * permission is obtained.
 */
#include "IndexBuild.h"

using namespace fnord;

namespace cm {

IndexBuild::IndexBuild(
    FeatureIndexWriter* feature_idx,
    FullIndex* full_idx) :
    feature_idx_(feature_idx),
    full_idx_(full_idx) {}

void IndexBuild::updateDocument(const IndexRequest& index_request) {
  auto doc = full_idx_->updateDocument(index_request);
}

void IndexBuild::commit() {
}

} // namespace cm
