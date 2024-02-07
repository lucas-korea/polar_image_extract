// 본 코드는 FLIR, LUCID 카메라에서 취득된 영상 데이터를(.bin) 이미지로 변환해주는 코드이다.
// 16bit 취득 데이터를 기준으로 작성하였다.
// 파일이름의 카메라 시리얼 넘버를 기준으로 RGB, polar 데이터를 구분
// python으로 작성된 뒤, chatGPT를 이용해 변환되었고, 일부 수정하였음

#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <filesystem>
#include <fstream>
#include <cstdio>
#include <stdio.h>
using namespace cv;
using namespace std;
namespace fs = std::filesystem;

void mkdirs(string path, string subdirectory);

// python string의 endswith() 함수를 구현하였음. 문자열이 특정 접미사로 끝나는지를 확인하는 함수
bool endsWith(std::string const& str, std::string const& suffix) {
    if (str.length() < suffix.length()) {
        return false;
    }
    return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

// RGB 이미지를 추출
void createRGBColorImage(const std::vector<uint8_t>& dataSet, string outputfilename) {
    int width = 2448;
    int height = 2048;

    cv::Mat image(height, width, CV_16U);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            // 8bit로 입력된 dataset 데이터에 대해, 16bit 데이터로 해석하기 위한 bit 계산
            image.at<uint16_t>(i, j) = dataSet[(i * width + j) * 2] + 255 * dataSet[(i * width + j) * 2 + 1];
        }
    }

    Mat rotatedImage;
    rotate(image, rotatedImage, ROTATE_180); // 카메라가 거꾸로 달려있어서, 돌려주는 작업

    Mat gray_image;
    rotatedImage.convertTo(gray_image, CV_16UC1);//채널에 대한 정보를 넣어주기 위한 컨버팅
    resize(rotatedImage, rotatedImage, Size(width / 4, height / 4)); // 모니터링 이미지 작게 만들어주기, 원본 사이즈는 커서 화면에 다 안보임
    imshow("rotatedImage", rotatedImage); // 추출하는 과정 모니터링용

    //resize(gray_image, gray_image, Size(width / 4, height / 4));
    //imshow("gray_image", gray_image);

    // bayer 구조의 이미지를, color 이미지로 바꿔주는 과정.
    Mat RGB_image(height / 2, width / 2, CV_16UC3);
    for (int i = 0; i < height / 2; i++) {
        for (int j = 0; j < width / 2; j++) {
            RGB_image.at<Vec3w>(i, j)[0] = gray_image.at<uint16_t>(i * 2, j * 2);
            RGB_image.at<Vec3w>(i, j)[1] = (gray_image.at<uint16_t>(i * 2 + 1, j * 2) / 2 + gray_image.at<uint16_t>(i * 2, j * 2 + 1) / 2);
            RGB_image.at<Vec3w>(i, j)[2] = gray_image.at<uint16_t>(i * 2 + 1, j * 2 + 1);
        }
    }
    string imagename = outputfilename + "_color.png";

    imwrite(imagename, RGB_image);
    resize(RGB_image, RGB_image, Size(width / 4, height / 4)); // 모니터링 이미지 작게 만들어주기, 원본 사이즈는 커서 화면에 다 안보임
    imshow("RGB_image", RGB_image); // 추출하는 과정 모니터링용
    waitKey(1);

    cout << imagename + " RGB color image created" << endl;
}

// polar 이미지를 추출
void createPolarSplitedColorImage(const std::vector<uint8_t>& dataSet, string path, string basename, string index) {
    int width = 2448;
    int height = 2048;

    cv::Mat image(height, width, CV_16U);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            // 8bit로 입력된 dataset 데이터에 대해, 16bit 데이터로 해석하기 위한 bit 계산
            image.at<uint16_t>(i, j) = dataSet[(i * width + j) * 2] + 255 * dataSet[(i * width + j) * 2 + 1];
        }
    }

    Mat rotatedImage;
    rotate(image, rotatedImage, ROTATE_180); // 카메라가 거꾸로 달려있어서, 돌려주는 작업

    Mat gray_image;
    rotatedImage.convertTo(gray_image, CV_16UC1); //채널에 대한 정보를 넣어주기 위한 컨버팅

    // 각 각도 이미지 생성
    Mat image_0(height / 4, width / 4, CV_16UC3);
    Mat image_45(height / 4, width / 4, CV_16UC3);
    Mat image_135(height / 4, width / 4, CV_16UC3);
    Mat image_90(height / 4, width / 4, CV_16UC3);

    // 기존에 이해하고 있던 픽셀 각도에 오류가 있었다. 왼쪽위(수직의 편광자) 가 90도이고, 시계방향으로 90, 45, 0 135이다.
    // 현재 카메라가 거꾸로 달려 있어서 이미지를 거꾸로 돌리고 난 뒤 맨 왼쪽 위가 0도 이고 시계방향으로 0,135,90,45 
    for (int i = 0; i < height / 4; ++i) {
        for (int j = 0; j < width / 4; ++j) {
            image_0.at<Vec3w>(i, j)[0] = gray_image.at<uint16_t>(i * 4, j * 4);
            image_0.at<Vec3w>(i, j)[1] = (gray_image.at<uint16_t>(i * 4 + 2, j * 4) / 2 + gray_image.at<uint16_t>(i * 4, j * 4 + 2) / 2);
            image_0.at<Vec3w>(i, j)[2] = gray_image.at<uint16_t>(i * 4 + 2, j * 4 + 2);

            image_135.at<Vec3w>(i, j)[0] = gray_image.at<uint16_t>(i * 4 + 1, j * 4);
            image_135.at<Vec3w>(i, j)[1] = (gray_image.at<uint16_t>(i * 4 + 2 + 1, j * 4) / 2 + gray_image.at<uint16_t>(i * 4 + 1, j * 4 + 2) / 2);
            image_135.at<Vec3w>(i, j)[2] = gray_image.at<uint16_t>(i * 4 + 2 + 1, j * 4 + 2);

            image_45.at<Vec3w>(i, j)[0] = gray_image.at<uint16_t>(i * 4, j * 4 + 1);
            image_45.at<Vec3w>(i, j)[1] = (gray_image.at<uint16_t>(i * 4 + 2, j * 4 + 1) / 2 + gray_image.at<uint16_t>(i * 4, j * 4 + 2 + 1) / 2);
            image_45.at<Vec3w>(i, j)[2] = gray_image.at<uint16_t>(i * 4 + 2, j * 4 + 2 + 1);

            image_90.at<Vec3w>(i, j)[0] = gray_image.at<uint16_t>(i * 4 + 1, j * 4 + 1);
            image_90.at<Vec3w>(i, j)[1] = (gray_image.at<uint16_t>(i * 4 + 2 + 1, j * 4 + 1) / 2 + gray_image.at<uint16_t>(i * 4 + 1, j * 4 + 2 + 1) / 2);
            image_90.at<Vec3w>(i, j)[2] = gray_image.at<uint16_t>(i * 4 + 2 + 1, j * 4 + 2 + 1);
        }
    }

    // 왼쪽 위가 0도 이고 시계방향으로 0,135,90,45 인것을 그대로 유지, 각도별 이미지 붙여서 all_angle 이미지 생성하는 과정
    Mat image0_135, image45_90, image_all;
    hconcat(image_0, image_135, image0_135);
    hconcat(image_45, image_90, image45_90);
    vconcat(image0_135, image45_90, image_all);

    string imagename0 = path + "\\0\\" + basename + "_" + index + "_0.png";
    string imagename45 = path + "\\45\\" + basename + "_" + index + "_45.png";
    string imagename90 = path + "\\90\\" + basename + "_" + index + "_90.png";
    string imagename135 = path + "\\135\\" + basename + "_" + index + "_135.png";
    string imagenameAll = path + "\\All_angle\\" + basename + "_" + index + "_all.png";

    imwrite(imagename0, image_0);
    imwrite(imagename45, image_45);
    imwrite(imagename90, image_90);
    imwrite(imagename135, image_135);
    imwrite(imagenameAll, image_all);

    resize(image_all, image_all, Size(width / 4, height / 4));
    imshow("image_all", image_all);
    waitKey(1);

    cout << path + "\\0\\" + basename + "_" + index + "_0.bmp" + " polar splited image created" << endl;
}

// python mkdirs() 함수. 폴더 생성 함수이다.
void mkdirs(string path, string subdirectory) {
    fs::path full_path = path + "\\" + subdirectory;
    if (!std::filesystem::exists(full_path)) {
        if (std::filesystem::create_directories(full_path)) {
            std::cout << full_path << "디렉토리 생성 성공" << std::endl;
        }
        else {
            std::cerr << full_path << "디렉토리 생성 실패" << std::endl;
        }
    }
    else {
        std::cout << "디렉토리 이미 존재함" << std::endl;
    }
}


int main() {
    string root_path = "E:\\240130_night"; // 카메라 취득 binary 데이터가 있는 폴더 

    vector<string> file_list; // root_path의 파일 목록 list 만들기, python os.listdir() 기능
    for (const auto& entry : fs::directory_iterator(root_path)) {
        string path{ entry.path().u8string() };
        cout << path << endl;
        file_list.push_back(path);
    }

    // file_list 중 파일명 기반하여 polar 데이터인, RGB 데이터 list 만들기
    vector<string> list_polar, list_RGB;
    for (string filename : file_list) {
        if (endsWith(filename, "232000061.bin")) {
            list_polar.push_back(filename);
            cout << "232000061" << filename << endl;
        }
        if (endsWith(filename, "22623682.bin")) {
            list_RGB.push_back(filename);
            cout << "22623682" << filename << endl;
        }
    }

    cout << "#######################################" << endl;
    cout << "########### polar data ################" << endl;
    cout << "#######################################" << endl;
    mkdirs(root_path, "polar");

    int cnt = 0;
    //vector<string> list_polar_slice(list_polar.begin() + 21, list_polar.end()); //중간에 에러난경우 벡터 슬라이스에서 계속
    for (string file : list_polar) {
        cout << cnt << " / " << list_polar.size() << " file" << endl;
        cnt++;
        string file_full_path = file;

        // 파일이름을 연월일_시분초_시리얼번호만 남기는 과정
        string base_name = file.substr(0, file.find_last_of("."));
        vector<std::string> tokens;
        string delimiter = "\\"; // 백슬래시로 문자열을 분할합니다.
        size_t pos = 0;
        while ((pos = base_name.find(delimiter)) != std::string::npos) {
            string token = base_name.substr(0, pos);
            tokens.push_back(token);
            base_name.erase(0, pos + delimiter.length());
        }


        // ifstream을 이용해 파일 읽기, 파일 크기 계산하기,  
        // seekg는 책갈피 역할. 파일 크기를 계산하기 위함이라서 끝으로 갔다가 다시 데이터를 순서대로 읽기 위해 처음으로 돌아옴
        ifstream input_file(file_full_path, ios::binary);
        input_file.seekg(0, ios::end); // 파일 끝으로 이동
        streamsize file_size = input_file.tellg();
        cout << file_full_path << '\t' << file_size << endl;
        input_file.seekg(0, ios::beg); // 파일 처음으로 이동

        // 해상도는 2448*2048 이지만 16bit이미지=2byte여서 곱하기 2를 해주어야 한다. 
        int image_number = file_size / (2448 * 2048 * 2);
        mkdirs(root_path, "polar\\0");
        mkdirs(root_path, "polar\\45");
        mkdirs(root_path, "polar\\90");
        mkdirs(root_path, "polar\\135");
        mkdirs(root_path, "polar\\All_angle");
        for (int i = 0; i < image_number; ++i) {
            vector<unsigned char> data(2448 * 2048 * 2); 
            input_file.read((char*)data.data(), 2448 * 2048 * 2);

            if (i % 2 == 0) { //파일을 전부 생성할지, 특정프레임만 만들지 판단
                cout << i << " / " << image_number << " images" << endl;

                createPolarSplitedColorImage(data, root_path + "\\polar", base_name, to_string(i));
            }
        }
        input_file.close();
    }

    //cout << "#######################################" << endl;
    //cout << "###########  RGB data  ################" << endl;
    //cout << "#######################################" << endl;
    //mkdirs(root_path, "RGB");
    //cnt = 0;
    //for (string file : list_RGB) {
    //    cout << cnt << " / " << list_RGB.size() << " file" << endl;
    //    cnt++;
    //    string file_full_path = file;

    //    // 파일이름을 연월일_시분초_시리얼번호만 남기는 과정
    //    string base_name = file.substr(0, file.find_last_of("."));
    //    vector<std::string> tokens;
    //    string delimiter = "\\"; // 백슬래시로 문자열을 분할합니다.
    //    size_t pos = 0;
    //    while ((pos = base_name.find(delimiter)) != std::string::npos) {
    //        string token = base_name.substr(0, pos);
    //        tokens.push_back(token);
    //        base_name.erase(0, pos + delimiter.length());
    //    }
    //    delimiter = ":";
    //    while ((pos = base_name.find(delimiter)) != std::string::npos) {
    //        string token = base_name.substr(0, pos);
    //        tokens.push_back(token);
    //        base_name.erase(0, pos + delimiter.length());
    //    }

    //    string outputFilename = root_path + "\\RGB\\" + base_name;


    //    // ifstream을 이용해 파일 읽기, 파일 크기 계산하기,  
    //    // seekg는 책갈피 역할. 파일 크기를 계산하기 위함이라서 끝으로 갔다가 다시 데이터를 순서대로 읽기 위해 처음으로 돌아옴
    //    ifstream input_file(file_full_path, ios::binary);
    //    input_file.seekg(0, ios::end); // 파일 끝으로 이동
    //    streamsize file_size = input_file.tellg();
    //    cout << file_full_path << '\t' << file_size << endl;
    //    input_file.seekg(0, ios::beg); // 파일 처음으로 이동

    //    // 해상도는 2448*2048 이지만 16bit이미지=2byte여서 곱하기 2를 해주어야 한다. 
    //    int image_number = file_size / (2448 * 2048 * 2);
    //    for (int i = 0; i < image_number; ++i) {
    //        vector<unsigned char> data(2448 * 2048 * 2);
    //        input_file.read((char*)data.data(), 2448 * 2048 * 2);
    //        if (i % 2 == 0) { //파일을 전부 생성할지, 특정프레임만 만들지 판단
    //            cout << i << " / " << image_number << " images" << endl;
    //            createRGBColorImage(data, outputFilename + "_" + to_string(i));
    //        }
    //    }
    //    input_file.close();
    //}

    return 0;
}