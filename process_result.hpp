/*
 * Copyright 2019 Xilinx Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Copyright 2019 H.Saido <saido.nv@gmail.com>
 */
#pragma once
#include <iostream>
#include <iomanip>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;


static vector<string> cityscapes_map{"car", "person", "cycle"};
static vector<string> bdd_map{
    "bike",  "bus",   "car",  "motor", "person",
    "rider", "light", "sign", "train", "truck"};
static vector<string> VOC_map{
    "aeroplane",   "bicycle", "bird",  "boat",      "bottle",
    "bus",         "car",     "cat",   "chair",     "cow",
    "diningtable", "dog",     "horse", "motorbike", "person",
    "pottedplant", "sheep",   "sofa",  "train",     "tvmonitor"};

static map<string, vector<string>> label_map = {
    {"yolov2_voc", VOC_map},
    {"yolov2_voc_pruned_0_66", VOC_map},
    {"yolov2_voc_pruned_0_71", VOC_map},
    {"yolov2_voc_pruned_0_77", VOC_map},
    {"yolov3_adas_pruned_0_9", cityscapes_map},
    {"yolov3_bdd", bdd_map},
    {"yolov3_voc", VOC_map},
    {"yolov3_voc_tf", VOC_map}
};

/**
 *@struct Result
 *@brief Struct of the result returned by the neuron network.
 */
struct Result {
  /// Width of input image.
  int width;
  /// Height of output image.
  int height;
  /**
   *@struct BoundingBox
   *@Brief Struct of detection result with a object
   */
  struct BoundingBox {
    /// classification.
    int label;
    /// confidence, the range from 0 to 1.
    float score;
    /// x-coordinate, x is normalized relative to the input image cols, its
    /// value range from 0 to 1.
    float x;
    /// y-coordinate, y is normalized relative to the input image rows, its
    /// value range from 0 to 1.
    float y;
    /// width, width is normalized relative to the input image cols, its value
    /// from 0 to 1.
    float width;
    /// height, height is normalized relative to the input image rows, its value
    /// range from 0 to 1.
    float height;
  };
  /// All objects, The vector of BoundingBox .
  vector<BoundingBox> bboxes;
};

/*
 * なんかよくわからんが、以下のコードを参考に、モデルごとに違う型を隠ぺい
 * Vitis-AI-Library/benchmark/include/vitis/ai/demo.hpp
 */
struct Model {
    explicit Model() {}
    virtual ~Model() {}
    virtual Result run(cv::Mat &input) = 0;
};

// Execute each lib run function
template <typename dpu_model_type_t>
struct DpuModel : public Model {
    DpuModel(unique_ptr<dpu_model_type_t> &&dpu_model)
        : Model{}, dpu_model_{move(dpu_model)} {}
    virtual ~DpuModel() {}
    Result run(cv::Mat &image) override {
        auto dpu_result = dpu_model_->run(image);
        Result *result = (Result *)&dpu_result;
        return *result;
    }
    unique_ptr<dpu_model_type_t> dpu_model_;
};

template <typename FactoryMethod>
unique_ptr<Model> create_dpu_model(const FactoryMethod &factory_method) {
    using dpu_model_type_t = typename decltype(factory_method())::element_type;
    return unique_ptr<Model>(
        new DpuModel<dpu_model_type_t>(factory_method())
    );
}


/*
 *   The color loops every 27 times,
 *    because each channel of RGB loop in sequence of "0, 127, 254"
 */
static cv::Scalar getColor(int label) {
    int c[3];
    for (int i = 1, j = 0; i <= 9; i *= 3, j++) {
        c[j] = ((label / i) % 3) * 127;
    }
    return cv::Scalar(c[2], c[1], c[0]);
}

static cv::Mat process_result(cv::Mat &image,
                              const vitis::ai::YOLOv3Result &result,
                              bool is_jpeg) {
    for (const auto bbox : result.bboxes) {
        int label = bbox.label;
        auto color = getColor(label);
        float xmin = bbox.x * image.cols + 1;
        float ymin = bbox.y * image.rows + 1;
        float xmax = xmin + bbox.width * image.cols;
        float ymax = ymin + bbox.height * image.rows;
        float confidence = bbox.score;
        if (xmax > image.cols) xmax = image.cols;
        if (ymax > image.rows) ymax = image.rows;

        cv::rectangle(image, cv::Point(xmin, ymin), cv::Point(xmax, ymax),
                      color, 1, 1, 0);
        
        stringstream ss;
        ss << label << ": " << fixed << setprecision(2) << confidence << endl;
        cv::putText(image, ss.str(), cv::Point(xmin, ymin-5),
            cv::FONT_HERSHEY_SIMPLEX, 0.5, color, 1);
    }

    return image;
}

static cv::Mat process_result_label(cv::Mat &image,
                              const Result &result,
                              const vector<string> labels, const float fps) {
    for (const auto bbox : result.bboxes) {
        int label = bbox.label;
        auto color = getColor(label);
        float xmin = bbox.x * image.cols + 1;
        float ymin = bbox.y * image.rows + 1;
        float xmax = xmin + bbox.width * image.cols;
        float ymax = ymin + bbox.height * image.rows;
        float confidence = bbox.score;
        if (xmax > image.cols) xmax = image.cols;
        if (ymax > image.rows) ymax = image.rows;

        cv::rectangle(image, cv::Point(xmin, ymin), cv::Point(xmax, ymax),
                      color, 1, 1, 0);
        
        stringstream ss;
        ss << labels[label] << ": " << fixed << setprecision(2) << confidence << endl;
        cv::putText(image, ss.str(), cv::Point(xmin, ymin-5),
            cv::FONT_HERSHEY_SIMPLEX, 0.5, color, 1);
    }

    stringstream fps_ss;
    fps_ss << "FPS: " << fixed << setprecision(2) << fps << endl;
    cv::putText(image, fps_ss.str(), cv::Point(2, 16),
        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(200, 48, 48), 2);

    return image;
}
