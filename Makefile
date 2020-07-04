OPENCV_LIBS="-lopencv_core -lopencv_video -lopencv_videoio -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui"
CXX = g++
CFLAGS = -std=c++11 -I. -Wall
LDFLAGS = -lopencv_core -lopencv_video -lopencv_videoio \
		-lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui \
		-lglog -pthread

AI_LDFLAGS = \
	-lvitis_ai_library-yolov3 \
	-lvitis_ai_library-yolov2 \
	-lvitis_ai_library-tfssd \
	-lvitis_ai_library-ssd

APPS = video_demo_app video_single_th_app

default:
		make all

all: $(APPS)

video_demo_app: video_demo_app.o
		$(CXX) -o $@ $^ $(LDFLAGS) -lvitis_ai_library-yolov3

video_single_th_app: video_single_th_app.o
		$(CXX) -o $@ $^ $(LDFLAGS) $(AI_LDFLAGS)

clean:
		$(RM) *.o $(APPS)
