# suspicious-activity-detection
Detects suspicious persons automatically from a video.

Requirements:
Install gcc and OpenCv before compiling the code.

Compile:
$ g++ -g -Wno-write-strings \`pkg-config --cflags opencv\` -o suspicious_loiter.o tracking.cpp Object.h Object.cpp utils.h utils.cpp \`pkg-config --libs opencv\`

Execute:
`$ ./suspicious_loiter.o <video_file>`
