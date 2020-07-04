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
    {"yolov3_adas_pruned_0_9", cityscapes_map},
    {"yolov3_bdd", bdd_map},
    {"yolov3_voc", VOC_map},
    {"yolov3_voc_tf", VOC_map}
};

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
                              const vitis::ai::YOLOv3Result &result,
                              vector<string> labels, float fps) {
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
