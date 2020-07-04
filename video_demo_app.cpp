/*
 * Copyright 2020 H.Saido <saido.nv@gmail.com>
 */
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <glog/logging.h> // vitis/ai/demo.hpp で必要になる
#include <vitis/ai/demo.hpp>
#include <vitis/ai/yolov3.hpp>

// 以下から拝借
// Vitis-AI-Library\overview\samples\yolov3\process_result.hpp
#include "./process_result.hpp"

using namespace std;

int main(int argc, char *argv[]) {
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
    string model = argv[1];

    return vitis::ai::main_for_video_demo(
        argc, argv,
        [model] {
            return vitis::ai::YOLOv3::create(model);
        },
        process_result, 2);
}
