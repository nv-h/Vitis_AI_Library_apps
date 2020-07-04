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
#include <vitis/ai/yolov3.hpp>
#include <vitis/ai/yolov2.hpp>
#include <vitis/ai/ssd.hpp>
#include <vitis/ai/tfssd.hpp>

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
static vector<string> ssdadas_map{ // cf_ssdadas_bdd_360_480_0.95_6.3G / cf_ssdtraffic_360_480_0.9_11.6G
    "background","car","cycle","person"};
static vector<string> ssdmobilenetv2_map{ // cf_ssdmobilenetv2_bdd_360_480_6.57G / cf_ssdpedestrian_coco_360_640_0.97_5.9G
    "background","person","rider","car","truck",
    "bus","train","motor","bike","sign","light"};
static vector<string> coco_map {
     /*1*/ "background",
     // https://github.com/amikelive/coco-labels/blob/master/coco-labels-paper.txt
     // https://github.com/amikelive/coco-labels/blob/master/coco-labels-2014_2017.txt
     // paper        //     2014_2017
    "person",        // 2   "person",
    "bicycle",       // 3   "bicycle",
    "car",           // 4   "car",
    "motorcycle",    // 5   "motorbike",
    "airplane",      // 6   "aeroplane",
    "bus",           // 7   "bus",
    "train",         // 8   "train",
    "truck",         // 9   "truck",
    "boat",          // 10  "boat",
    "traffic light", // 11  "traffic light",
    "fire hydrant",  // 12  "fire hydrant",
    "street sign",   // 13  "stop sign",
    "stop sign",     // 14  "parking meter",
    "parking meter", // 15  "bench",
    "bench",         // 16  "bird",
    "bird",          // 17  "cat",
    "cat",           // 18  "dog",
    "dog",           // 19  "horse",
    "horse",         // 20  "sheep",
    "sheep",         // 21  "cow",
    "cow",           // 22  "elephant",
    "elephant",      // 23  "bear",
    "bear",          // 24  "zebra",
    "zebra",         // 25  "giraffe",
    "giraffe",       // 26  "backpack",
    "hat",           // 27  "umbrella",
    "backpack",      // 28  "handbag",
    "umbrella",      // 29  "tie",
    "shoe",          // 30  "suitcase",
    "eye glasses",   // 31  "frisbee",
    "handbag",       // 32  "skis",
    "tie",           // 33  "snowboard",
    "suitcase",      // 34  "sports ball",
    "frisbee",       // 35  "kite",
    "skis",          // 36  "baseball bat",
    "snowboard",     // 37  "baseball glove",
    "sports ball",   // 38  "skateboard",
    "kite",          // 39  "surfboard",
    "baseball bat",  // 40  "tennis racket",
    "baseball glove",// 41  "bottle",
    "skateboard",    // 42  "wine glass",
    "surfboard",     // 43  "cup",
    "tennis racket", // 44  "fork",
    "bottle",        // 45  "knife",
    "plate",         // 46  "spoon",
    "wine glass",    // 47  "bowl",
    "cup",           // 48  "banana",
    "fork",          // 49  "apple",
    "knife",         // 50  "sandwich",
    "spoon",         // 51  "orange",
    "bowl",          // 52  "broccoli",
    "banana",        // 53  "carrot",
    "apple",         // 54  "hot dog",
    "sandwich",      // 55  "pizza",
    "orange",        // 56  "donut",
    "broccoli",      // 57  "cake",
    "carrot",        // 58  "chair",
    "hot dog",       // 59  "sofa",
    "pizza",         // 60  "pottedplant",
    "donut",         // 61  "bed",
    "cake",          // 62  "diningtable",
    "chair",         // 63  "toilet",
    "couch",         // 64  "tvmonitor",
    "potted plant",  // 65  "laptop",
    "bed",           // 66  "mouse",
    "mirror",        // 67  "remote",
    "dining table",  // 68  "keyboard",
    "window",        // 69  "cell phone",
    "desk",          // 70  "microwave",
    "toilet",        // 71  "oven",
    "door",          // 72  "toaster",
    "tv",            // 73  "sink",
    "laptop",        // 74  "refrigerator",
    "mouse",         // 75  "book",
    "remote",        // 76  "clock",
    "keyboard",      // 77  "vase",
    "cell phone",    // 78  "scissors",
    "microwave",     // 79  "teddy bear",
    "oven",          // 80  "hair drier",
    "toaster",       // 81  "toothbrush"
    "sink",          // 82 
    "refrigerator",  // 83 
    "blender",       // 84 
    "book",          // 85 
    "clock",         // 86 
    "vase",          // 87 
    "scissors",      // 88 
    "teddy bear",    // 89 
    "hair drier",    // 90 
    "toothbrush",    // 91 
    "hair brush"     // 92 
    };

static map<string, vector<string>> label_map = {
    {"yolov2_voc", VOC_map},
    {"yolov2_voc_pruned_0_66", VOC_map},
    {"yolov2_voc_pruned_0_71", VOC_map},
    {"yolov2_voc_pruned_0_77", VOC_map},
    {"yolov3_adas_pruned_0_9", cityscapes_map},
    {"yolov3_bdd", bdd_map},
    {"yolov3_voc", VOC_map},
    {"yolov3_voc_tf", VOC_map},
    {"ssd_adas_pruned_0_95", ssdadas_map},
    {"ssd_mobilenet_v2", ssdmobilenetv2_map},
    {"ssd_pedestrain_pruned_0_97", ssdmobilenetv2_map},
    {"ssd_traffic_pruned_0_9", ssdadas_map},
    {"ssd_mobilenet_v1_coco_tf", coco_map},
    {"ssd_mobilenet_v2_coco_tf", coco_map},
    {"ssd_resnet_50_fpn_coco_tf", coco_map}
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
                              const Result &result,
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

static cv::Mat process_result_yolov3(
    cv::Mat &image, const vitis::ai::YOLOv3Result &result, bool is_jpeg) {
    Result *result_p = (Result *)&result;
    return process_result(image, *result_p, is_jpeg);
}

static cv::Mat process_result_yolov2(
    cv::Mat &image, const vitis::ai::YOLOv2Result &result, bool is_jpeg) {
    Result *result_p = (Result *)&result;
    return process_result(image, *result_p, is_jpeg);
}

static cv::Mat process_result_ssd(
    cv::Mat &image, const vitis::ai::SSDResult &result, bool is_jpeg) {
    Result *result_p = (Result *)&result;
    return process_result(image, *result_p, is_jpeg);
}

static cv::Mat process_result_tfssd(
    cv::Mat &image, const vitis::ai::TFSSDResult &result, bool is_jpeg) {
    Result *result_p = (Result *)&result;
    return process_result(image, *result_p, is_jpeg);
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
        if (labels.size() < label) {
            ss <<               "??? [" << label << "]: " << fixed << setprecision(2) << confidence << endl;
            cout << "Warning! label: " << label << endl;
        }
        else {
            ss << labels[label] << " [" << label << "]: " << fixed << setprecision(2) << confidence << endl;
        }
        
        cv::putText(image, ss.str(), cv::Point(xmin, ymin-5),
            cv::FONT_HERSHEY_SIMPLEX, 0.5, color, 1);
    }

    stringstream fps_ss;
    fps_ss << "FPS: " << fixed << setprecision(2) << fps << endl;
    cv::putText(image, fps_ss.str(), cv::Point(2, 16),
        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(200, 48, 48), 2);

    return image;
}
