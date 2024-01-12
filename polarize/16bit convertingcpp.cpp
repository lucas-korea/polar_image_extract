//#include <iostream>
//#include <opencv2/opencv.hpp>
//#include <string>
//#include <filesystem>
//#include <fstream>
//#include <cstdio>
//#include <stdio.h>
//using namespace cv;
//using namespace std;
//namespace fs = std::filesystem;
//
//void mkdirs(string path, string subdirectory);
//
//bool endsWith(std::string const& str, std::string const& suffix) {
//    if (str.length() < suffix.length()) {
//        return false;
//    }
//    return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
//}
//
//
//void createRGBColorImage(const std::vector<uint8_t>& dataSet, string outputfilename) {
//    int width = 2448;
//    int height = 2048;
//
//    cv::Mat image(height, width, CV_16U);
//    for (int i = 0; i < height; i++) {
//        for (int j = 0; j < width; j++) {
//            image.at<uint16_t>(i, j) = dataSet[(i * width + j) * 2] + 255 * dataSet[(i * width + j) * 2 + 1];
//        }
//    }
//
//    Mat rotatedImage;
//    rotate(image, rotatedImage, ROTATE_180);
//
//    Mat gray_image;
//    rotatedImage.convertTo(gray_image, CV_16UC1);
//    resize(rotatedImage, rotatedImage, Size(width / 4, height / 4));
//    imshow("rotatedImage", rotatedImage);
//    //resize(gray_image, gray_image, Size(width / 4, height / 4));
//    //imshow("gray_image", gray_image);
//    
//    Mat RGB_image(height / 2, width / 2, CV_16UC3);
//    for (int i = 0; i < height / 2; i++) {
//        for (int j = 0; j < width / 2; j++) {
//            RGB_image.at<Vec3w>(i, j)[0] = gray_image.at<uint16_t>(i * 2, j * 2);
//            RGB_image.at<Vec3w>(i, j)[1] = (gray_image.at<uint16_t>(i * 2 + 1, j * 2) / 2 + gray_image.at<uint16_t>(i * 2, j * 2 + 1) / 2);
//            RGB_image.at<Vec3w>(i, j)[2] = gray_image.at<uint16_t>(i * 2 + 1, j * 2 + 1);
//        }
//    }
//    string imagename = outputfilename + "_color.png";
//
//    imwrite(imagename, RGB_image);
//    resize(RGB_image, RGB_image, Size(width / 4, height / 4));
//    imshow("RGB_image", RGB_image);
//    waitKey(1);
//
//    cout << imagename + " RGB color image created" << endl;
//}
//
//void createPolarSplitedColorImage(const std::vector<uint8_t>& dataSet, string path, string basename, string index) {
//    int width = 2448;
//    int height = 2048;
//
//    cv::Mat image(height, width, CV_16U);
//    for (int i = 0; i < height; i++) {
//        for (int j = 0; j < width; j++) {
//            image.at<uint16_t>(i, j) = dataSet[(i * width + j)*2] + 255 * dataSet[(i * width + j)*2 + 1];
//        }
//    }
//
//    Mat rotatedImage;
//    rotate(image, rotatedImage, ROTATE_180);
//
//    Mat gray_image;
//    rotatedImage.convertTo(gray_image, CV_16UC1);
//
//    Mat image_0(height / 4, width / 4, CV_16UC3);
//    Mat image_45(height / 4, width / 4, CV_16UC3);
//    Mat image_135(height / 4, width / 4, CV_16UC3);
//    Mat image_90(height / 4, width / 4, CV_16UC3);
//
//    // 현재 카메라가 거꾸로 달려 있어서 이미지를 거꾸로 돌리는 과정을 고려하지 않고 각도 분할 하였음. 거꾸로 촬영된 이미지 기준, 왼쪽 위가 0도이고 
//    // 시계방향으로 0, 45, 90, 135인데, 지금은 180도 돌려놓은 이미지 기준으로 왼쪽위를 0도로 간주하였음. 이는 오류. ver2에서 수정 예정
//    for (int i = 0; i < height / 4; ++i) {
//        for (int j = 0; j < width / 4; ++j) {
//            image_0.at<Vec3w>(i, j)[0] = gray_image.at<uint16_t>(i * 4, j * 4);
//            image_0.at<Vec3w>(i, j)[1] = (gray_image.at<uint16_t>(i * 4 + 2, j * 4) / 2 + gray_image.at<uint16_t>(i * 4, j * 4 + 2) / 2);
//            image_0.at<Vec3w>(i, j)[2] = gray_image.at<uint16_t>(i * 4 + 2, j * 4 + 2);
//
//            image_45.at<Vec3w>(i, j)[0] = gray_image.at<uint16_t>(i * 4 + 1, j * 4);
//            image_45.at<Vec3w>(i, j)[1] = (gray_image.at<uint16_t>(i * 4 + 2 + 1, j * 4) / 2 + gray_image.at<uint16_t>(i * 4 + 1, j * 4 + 2) / 2);
//            image_45.at<Vec3w>(i, j)[2] = gray_image.at<uint16_t>(i * 4 + 2 + 1, j * 4 + 2);
//
//            image_135.at<Vec3w>(i, j)[0] = gray_image.at<uint16_t>(i * 4, j * 4 + 1);
//            image_135.at<Vec3w>(i, j)[1] = (gray_image.at<uint16_t>(i * 4 + 2, j * 4 + 1) / 2 + gray_image.at<uint16_t>(i * 4, j * 4 + 2 + 1) / 2);
//            image_135.at<Vec3w>(i, j)[2] = gray_image.at<uint16_t>(i * 4 + 2, j * 4 + 2 + 1);
//
//            image_90.at<Vec3w>(i, j)[0] = gray_image.at<uint16_t>(i * 4 + 1, j * 4 + 1);
//            image_90.at<Vec3w>(i, j)[1] = (gray_image.at<uint16_t>(i * 4 + 2 + 1, j * 4 + 1) / 2 + gray_image.at<uint16_t>(i * 4 + 1, j * 4 + 2 + 1) / 2);
//            image_90.at<Vec3w>(i, j)[2] = gray_image.at<uint16_t>(i * 4 + 2 + 1, j * 4 + 2 + 1);
//        }
//    }
//
//    Mat image0_45, image135_90, image_all;
//    hconcat(image_0, image_45, image0_45);
//    hconcat(image_135, image_90, image135_90);
//    vconcat(image0_45, image135_90, image_all);
//
//    string imagename0 = path + "\\0\\" + basename + "_" + index + "_0.png";
//    string imagename45 = path + "\\45\\" + basename + "_" + index + "_45.png";
//    string imagename90 = path + "\\90\\" + basename + "_" + index + "_90.png";
//    string imagename135 = path + "\\135\\" + basename + "_" + index + "_135.png";
//    string imagenameAll = path + "\\All_angle\\" + basename + "_" + index + "_all.png";
//
//    imwrite(imagename0, image_0);
//    imwrite(imagename45, image_45);
//    imwrite(imagename90, image_90);
//    imwrite(imagename135, image_135);
//    imwrite(imagenameAll, image_all);
//
//    resize(image_all, image_all, Size(width / 4, height / 4));
//    imshow("image_all", image_all);
//    waitKey(1);
//
//    cout << path + "\\0\\" + basename + "_" + index + "_0.bmp" + " polar splited image created" << endl;
//}
//
//void mkdirs(string path, string subdirectory) {
//    fs::path full_path = path + "\\" + subdirectory;
//    if (!std::filesystem::exists(full_path)) {
//        if (std::filesystem::create_directories(full_path)) {
//            std::cout << full_path << "디렉토리 생성 성공" << std::endl;
//        }
//        else {
//            std::cerr << full_path << "디렉토리 생성 실패" << std::endl;
//        }
//    }
//    else {
//        std::cout << "디렉토리 이미 존재함" << std::endl;
//    }
//}
//
//
//int main() {
//    string root_path = "F:\\20231122";
//    vector<string> list1; // Fill the list of filenames in the directory
//    for (const auto& entry : fs::directory_iterator(root_path)) {
//        string path{ entry.path().u8string() };
//        cout << path << endl;
//        list1.push_back(path);
//    }
//    
//    vector<string> list_polar, list_RGB;
//    for (string filename : list1) {
//        if (endsWith(filename, "232000061.bin")) {
//            list_polar.push_back(filename);
//            cout << "232000061" << filename << endl;
//        }
//        if (endsWith(filename, "22623682.bin")) {
//            list_RGB.push_back(filename);
//            cout << "22623682" << filename << endl;
//        }
//    }
//
//    cout << "#######################################" << endl;
//    cout << "########### polar data ################" << endl;
//    cout << "#######################################" << endl;
//    mkdirs(root_path, "polar");
//
//    int cnt = 0;
//    //vector<string> list_polar_slice(list_polar.begin() + 21, list_polar.end()); //중간에 에러난경우 벡터 슬라이스에서 계속
//    for (string file : list_polar) {
//        cout << cnt << " / " << list_polar.size() << " file" << endl;
//        cnt++;
//        string file_full_path = file;
//
//        // 파일이름을 연월일_시분초_시리얼번호만 남기는 과정
//        string base_name = file.substr(0, file.find_last_of("."));
//        vector<std::string> tokens;
//        string delimiter = "\\"; // 백슬래시로 문자열을 분할합니다.
//        size_t pos = 0;
//        while ((pos = base_name.find(delimiter)) != std::string::npos) {
//            string token = base_name.substr(0, pos);
//            tokens.push_back(token);
//            base_name.erase(0, pos + delimiter.length());
//        }
//
//
//
//        ifstream input_file(file_full_path, ios::binary);
//        input_file.seekg(0, ios::end); // 파일 끝으로 이동
//        streamsize file_size = input_file.tellg();
//        cout << file_full_path << '\t' << file_size << endl;
//        input_file.seekg(0, ios::beg); // 파일 처음으로 이동
//
//        int image_number = file_size / (2448 * 2048 * 2);
//        mkdirs(root_path, "polar\\0");
//        mkdirs(root_path, "polar\\45");
//        mkdirs(root_path, "polar\\90");
//        mkdirs(root_path, "polar\\135");
//        mkdirs(root_path, "polar\\All_angle");
//        for (int i = 0; i < image_number; ++i) {
//            vector<unsigned char> data(2448 * 2048 * 2);
//            input_file.read((char*)data.data(), 2448 * 2048 * 2);
//
//            if (i % 10 == 0) {
//                cout << i << " / " << image_number << " images" << endl;
//
//                createPolarSplitedColorImage(data, root_path + "\\polar", base_name, to_string(i));
//            }
//        }
//        input_file.close();
//    }
//
//    cout << "#######################################" << endl;
//    cout << "###########  RGB data  ################" << endl;
//    cout << "#######################################" << endl;
//    mkdirs(root_path, "RGB");
//    cnt = 0;
//    //for (string file : list_RGB) {
//    //    cout << cnt << " / " << list_RGB.size() << " file" << endl;
//    //    cnt++;
//    //    string file_full_path = file;
//
//    //    // 파일이름을 연월일_시분초_시리얼번호만 남기는 과정
//    //    string base_name = file.substr(0, file.find_last_of("."));
//    //    vector<std::string> tokens;
//    //    string delimiter = "\\"; // 백슬래시로 문자열을 분할합니다.
//    //    size_t pos = 0;
//    //    while ((pos = base_name.find(delimiter)) != std::string::npos) {
//    //        string token = base_name.substr(0, pos);
//    //        tokens.push_back(token);
//    //        base_name.erase(0, pos + delimiter.length());
//    //    }
//    //    delimiter = ":";
//    //    while ((pos = base_name.find(delimiter)) != std::string::npos) {
//    //        string token = base_name.substr(0, pos);
//    //        tokens.push_back(token);
//    //        base_name.erase(0, pos + delimiter.length());
//    //    }
//
//    //    string outputFilename = root_path + "\\RGB\\" + base_name;
//
//    //    ifstream input_file(file_full_path, ios::binary);
//    //    input_file.seekg(0, ios::end); // 파일 끝으로 이동
//    //    streamsize file_size = input_file.tellg();
//    //    cout << file_full_path << '\t' << file_size << endl;
//    //    input_file.seekg(0, ios::beg); // 파일 처음으로 이동
//
//    //    int image_number = file_size / (2448 * 2048 * 2);
//
//    //    for (int i = 0; i < image_number; ++i) {
//    //        vector<unsigned char> data(2448 * 2048 * 2);
//    //        input_file.read((char*)data.data(), 2448 * 2048 * 2);
//
//    //        if (i % 2 == 0) {
//    //            cout << i << " / " << image_number << " images" << endl;
//    //            createRGBColorImage(data, outputFilename + "_" + to_string(i));
//    //        }
//    //    }
//    //    input_file.close();
//    //}
//
//    return 0;
//}