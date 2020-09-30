// Tencent is pleased to support the open source community by making TNN available.
//
// Copyright (C) 2020 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

#include "onnx/onnx_utils.h"
#include "tnn/interpreter/tnn/objseri.h"
#include "tools/converter/source/onnx/onnx_base_converter.h"

namespace TNN_CONVERTER {
DECLARE_OP_CONVERTER(Upsample);

std::string OnnxUpsampleConverter::TNNOpType(const onnx::NodeProto &node, bool quantized_model) {
    return "Upsample";
}

TNN_NS::ActivationType OnnxUpsampleConverter::ActivationType(const onnx::NodeProto &node) {
    return TNN_NS::ActivationType_None;
}

TNN_NS::Status OnnxUpsampleConverter::exec(TNN_NS::NetStructure &net_structure, TNN_NS::NetResource &net_resource,
                                            const onnx::NodeProto &node,
                                            std::map<std::string, const onnx::TensorProto *> proxy_initializers_map,
                                            std::map<std::string, std::shared_ptr<OnnxProxyNode>> proxy_nodes,
                                            bool &quantized_model) {
    const std::string &onnx_op = node.op_type();
    auto param                 = new TNN_NS::UpsampleLayerParam;
    auto cur_layer             = net_structure.layers.back();
    cur_layer->param           = std::shared_ptr<TNN_NS::LayerParam>(param);
    param->type                = cur_layer->type_str;
    param->name                = cur_layer->name;
    param->quantized           = false;
    param->mode                = 0;

    auto model     = GetAttributeString(node, "mode", "nearest");
    if("linear" == model) {
        param->mode = 1;
    }

    const auto &shape_name           = node.input(1);
    const auto &shape_node           = FindNodeProto(shape_name, proxy_nodes);
    const auto &shape_tensor         = GetAttributeTensor(*shape_node, "scales");
    const int64_t *shape_tensor_data = (int64_t *)GetTensorProtoData(shape_tensor);
    const int shape_tensor_size      = GetTensorProtoDataSize(shape_tensor);

    for (int i = 0; i < shape_tensor_size; i++) {
        param->scales[i] = shape_tensor_data[i];
    }

    cur_layer->inputs.resize(1);
    cur_layer->inputs[0] = node.input(0);

    return TNN_NS::TNN_CONVERT_OK;
}

REGISTER_CONVERTER(Upsample, Upsample);

}  // namespace TNN_CONVERTER
