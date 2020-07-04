# Vitis AI Library Applications

Vitis AI Libraryを使用したアプリケーションを作成、動作確認する。  
ハードウェア環境の構築などは以下を参照。

* [DNNDK on Vitis AI on Ultra96v2](https://qiita.com/nv-h/items/7525c9319087a3f51755)
* [Vitis AI Library on Ultra96v2](https://qiita.com/nv-h/items/8f3b05e4e47928718992)

# Usage

```sh
video_demo_app <model_name> [-t <thread_num>]
```

* `model_name` :使用できるモデルは、`/usr/share/vitis_ai_library/models`にあるものだけと思われる。現状、以下に対応。  
    + Yolo v3: `yolov3_adas_pruned_0_9` `yolov3_bdd` `yolov3_voc` `yolov3_voc_tf`
* `thread_num`: Xilinxが用意しているデモライブラリでのスレッド指定。DPUとのデータのやり取りが高速になる。


# Performance (fps) on ultra96v2

ultra96v2ではDPUが一つしか実装できないのでマルチスレッドの効果はほとんどない？

Using USB Web Camera C615 @640*480 (X forwarding)

|          model           | -t 1 | -t 2 | -t 4 |
|--------------------------|------|------|------|
| `yolov3_adas_pruned_0_9` |   12 |   12 |   12 |
| `yolov3_bdd`             |    5 |    5 |    5 |
| `yolov3_voc`             |    5 |    5 |    5 |
| `yolov3_voc_tf`          |    5 |    5 |    5 |