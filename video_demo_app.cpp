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

// HEIGHT=360は対応していないカメラがあるので、VGAにした。
#define WIDTH 640
#define HEIGHT 480
#define FPS 60

using namespace std;

void print_usage_and_exit(char *argv[]) {
    cerr << "usage: " << argv[0] << " <model> <video_num> [-t <thread_num>]" << endl;
    cerr << "" << endl;
    cerr << "  model: " << endl;
    cerr << "    yolov2_voc, yolov2_voc_pruned_0_66, yolov2_voc_pruned_0_71, yolov2_voc_pruned_0_77" << endl;
    cerr << "    yolov3_adas_pruned_0_9, yolov3_bdd, yolov3_voc, yolov3_voc_tf" << endl;
    cerr << "    ssd_adas_pruned_0_95, ssd_mobilenet_v2, ssd_pedestrain_pruned_0_97, ssd_traffic_pruned_0_9" << endl;
    cerr << "    ssd_mobilenet_v1_coco_tf, ssd_mobilenet_v2_coco_tf, ssd_resnet_50_fpn_coco_tf" << endl;
    cerr << "" << endl;
    cerr << "  video_num: integer (X of /dev/videoX)" << endl;
    cerr << "  thread_num: thread number for DPU processing" << endl;
    cerr << "" << endl;
    exit(-1);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        print_usage_and_exit(argv);
    }

    // モデルとして使用できる文字列は、以下に格納されているモデルだけと思われる。
    //   /usr/share/vitis_ai_library/models
    // 現状のyolov3の場合、以下の4つ
    //   yolov3_adas_pruned_0_9, yolov3_bdd, yolov3_voc, yolov3_voc_tf
    string model = argv[1];

    // 一旦、カメラの設定を行っておく。
    // ここで、特にfpsを設定しないと30fpsで動くため
    // ただし、demo.hpp内で個別に設定している模様
    int video_num = atoi(argv[2]);
    cv::VideoCapture cap(video_num);
    if (!cap.isOpened()) {
        cerr << "Error! Could not opened: /dev/video" << video_num << endl;
        print_usage_and_exit(argv);
    }
    cap.set(CV_CAP_PROP_FRAME_WIDTH, WIDTH);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, HEIGHT);
    cap.set(CV_CAP_PROP_FPS, FPS);
    cap.release(); // demo.hpp内で使うので、ここでは閉じる

    /* main_for_video_demo()は、Xilinxが用意しているデモ用の関数
     * int main_for_video_demo(
     *     int argc, char *argv[],
     *     const FactoryMethod &factory_method,
     *     const ProcessResult &process_result,
     *     int start_pos = 1)
     *
     *   params: 
     *       argc, argv: -t スレッド数, -c チャネル数(ただし機能しない？)
     *       factory_method: vitis::ai::XXXX::create(model) で作られるオブジェクト
     *       process_result: 結果を処理する関数を突っ込む
     *                       多分、モデルごとに結果が違うために、専用の結果処理関数が
     *                       必要なのでこのような構成になっている
     *       start_pos: int 突っ込んだ引数の解析開始位置
     *                      ラッパー関数(今回はこのmain())で引数を消費したら、その分ずらす
     *                      この例では、modelで一つ消費しているので2になる(0は無視)
     */
    if (model.compare(0, 6, "yolov3") == 0) {
        return vitis::ai::main_for_video_demo(
            argc, argv, [model] {return vitis::ai::YOLOv3::create(model);},
            process_result_yolov3, 2);
    }
    else if (model.compare(0, 6, "yolov2") == 0) {
    return vitis::ai::main_for_video_demo(
        argc, argv, [model] {return vitis::ai::YOLOv2::create(model);},
        process_result_yolov2, 2);
    }
    else if (model.compare(0, 3, "ssd") == 0) {
        if (model.compare(model.size()-3, 3, "_tf") == 0) {
            return vitis::ai::main_for_video_demo(
                argc, argv, [model] {return vitis::ai::TFSSD::create(model);},
                process_result_tfssd, 2);
        }
        else {
            return vitis::ai::main_for_video_demo(
                argc, argv, [model] {return vitis::ai::SSD::create(model);},
                process_result_ssd, 2);
        }
    }
    else {
        cerr << "Error! Unsupported model: " << model <<  endl;
        print_usage_and_exit(argv);
    }
}
