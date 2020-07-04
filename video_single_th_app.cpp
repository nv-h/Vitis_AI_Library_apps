/*
 * Copyright 2020 H.Saido <saido.nv@gmail.com>
 */
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <vitis/ai/demo.hpp>
#include <vitis/ai/yolov3.hpp>

// 以下から拝借
// Vitis-AI-Library\overview\samples\yolov3\process_result.hpp
#include "./process_result.hpp"

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "usage: " << argv[0] << " <model_name> <video_num>" << endl;
        cerr << "       model_name: yolov3_adas_pruned_0_9, yolov3_bdd, yolov3_voc, yolov3_voc_tf" << endl;
        cerr << "       video_num: integer (X of /dev/videoX)" << endl;
        return -1;
    }

    // モデルとして使用できる文字列は、以下に格納されているモデルだけと思われる。
    //   /usr/share/vitis_ai_library/models
    // 現状のyolov3の場合、以下の4つ
    //   yolov3_adas_pruned_0_9, yolov3_bdd, yolov3_voc, yolov3_voc_tf
    string model_name = argv[1];
    vector<string> labels = label_map[model_name];
    // 強制的にinteger化
    int video_num = atoi(argv[2]);

    cv::VideoCapture cap(video_num);
    if (!cap.isOpened()) {
        cerr << "Error! Could not opened: /dev/video" << video_num <<  endl;
        return -1;
    }
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 360);

    cv::Mat frame;

    auto dpu_model = vitis::ai::YOLOv3::create(model_name);

    while (true) {
        cap >> frame;
        auto dpu_result = dpu_model->run(frame);

        auto result_frame = process_result_label(frame, dpu_result, labels);

        cv::imshow("", result_frame);
        int key = cv::waitKey(1);
        if (key == 'q') {
            break;
        }

    }
}
