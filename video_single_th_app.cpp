/*
 * Copyright 2020 H.Saido <saido.nv@gmail.com>
 */
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <vitis/ai/demo.hpp>
#include <vitis/ai/yolov3.hpp>
#include <vitis/ai/yolov2.hpp>
#include <vitis/ai/ssd.hpp>
#include <vitis/ai/tfssd.hpp>

// 以下から拝借
// Vitis-AI-Library\overview\samples\yolov3\process_result.hpp
#include "./process_result.hpp"

// Xilinxデモに合わせたが、特に意味はない。
// ただし、大きくしすぎると表示に時間がかかる。
#define WIDTH 640
#define HEIGHT 360

using namespace std;

void print_usage_and_exit(char *argv[]) {
    cerr << "usage: " << argv[0] << " <model_name> <video_num>" << endl;
    cerr << "" << endl;
    cerr << "  model_name: " << endl;
    cerr << "    yolov2_voc, yolov2_voc_pruned_0_66, yolov2_voc_pruned_0_71, yolov2_voc_pruned_0_77" << endl;
    cerr << "    yolov3_adas_pruned_0_9, yolov3_bdd, yolov3_voc, yolov3_voc_tf" << endl;
    cerr << "    ssd_adas_pruned_0_95, ssd_mobilenet_v2, ssd_pedestrain_pruned_0_97, ssd_traffic_pruned_0_9" << endl;
    cerr << "    ssd_mobilenet_v1_coco_tf, ssd_mobilenet_v2_coco_tf, ssd_resnet_50_fpn_coco_tf" << endl;
    cerr << "" << endl;
    cerr << "  video_num: integer (X of /dev/videoX)" << endl;
    cerr << "" << endl;
    exit(-1);
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        print_usage_and_exit(argv);
    }

    // モデルとして使用できる文字列は、以下に格納されているモデルだけと思われる。
    //   /usr/share/vitis_ai_library/models
    // 例えばyolov3の場合、以下の4つ
    //   yolov3_adas_pruned_0_9, yolov3_bdd, yolov3_voc, yolov3_voc_tf
    string model_name = argv[1];
    vector<string> labels = label_map[model_name];
    // 強制的にinteger化
    int video_num = atoi(argv[2]);

    cv::VideoCapture cap(video_num);
    if (!cap.isOpened()) {
        cerr << "Error! Could not opened: /dev/video" << video_num << endl;
        print_usage_and_exit(argv);
    }

    cap.set(CV_CAP_PROP_FRAME_WIDTH, WIDTH);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, HEIGHT);

    cv::Mat frame;
    cv::TickMeter tm;
    float fps = 0.0;

    // ネットワークモデルごとに固有のオブジェクトを呼ぶ必要がある
    unique_ptr<Model> dpu_model;
    if (model_name.compare(0, 6, "yolov3") == 0) {
        dpu_model = create_dpu_model(
            [model_name] {return vitis::ai::YOLOv3::create(model_name);}
        );
    }
    else if (model_name.compare(0, 6, "yolov2") == 0) {
        dpu_model = create_dpu_model(
            [model_name] {return vitis::ai::YOLOv2::create(model_name);}
        );
    }
    else if (model_name.compare(0, 3, "ssd") == 0) {
        if (model_name.compare(model_name.size()-3, 3, "_tf") == 0) {
            dpu_model = create_dpu_model(
                [model_name] {return vitis::ai::TFSSD::create(model_name);}
            );
        }
        else {
            dpu_model = create_dpu_model(
                [model_name] {return vitis::ai::SSD::create(model_name);}
            );
        }
    }
    else {
        cerr << "Error! Unsupported model: " << model_name <<  endl;
        print_usage_and_exit(argv);
    }

    // window size follow manual change
    // (Default setting is WINDOW_AUTOSIZE that fixed size window)
    cv::namedWindow(model_name, cv::WINDOW_NORMAL);
    cv::resizeWindow(model_name, WIDTH, HEIGHT);

    tm.start();
    while (true) {
        cap >> frame;

        // ネットワークモデルごとに返ってくる結果が違う
        // 物体検知なら確認した限りは同じだったので、汎用の結果構造体にキャストする
        // これにより、process_result_label()をモデルに依存しない形にできる
        auto dpu_result = dpu_model->run(frame);
        Result *result = (Result *)&dpu_result;

        auto result_frame = process_result_label(frame, *result, labels, fps);

        cv::imshow(model_name, result_frame);
        int key = cv::waitKey(1);
        if (key == 'q') {
            break;
        }

        if(tm.getCounter() > 0) {
            fps = tm.getCounter() / tm.getTimeSec();
        }
        tm.stop();
        tm.start();
    }

    cap.release();
}
