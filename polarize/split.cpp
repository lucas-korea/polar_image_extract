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
//int main() {
//    // Replace with the actual path to your directory
//    std::string path = "E:\\15000set\\polar_drive_image_all_angle_interpolation";
//    std::vector<std::string> file_list;
//    for (const auto& entry : std::filesystem::directory_iterator(path)) {
//        file_list.push_back(entry.path().string());
//    }
//    // Create subdirectories
//    std::filesystem::create_directories(path + "\\0");
//    std::filesystem::create_directories(path + "\\45");
//    std::filesystem::create_directories(path + "\\90");
//    std::filesystem::create_directories(path + "\\135");
//    int cnt = 0;
//    for (const auto& file : file_list) {
//        std::cout << file <<'\t' << cnt++ << std::endl;
//
//        // Read the image
//        cv::Mat img = cv::imread(file, -1);
//
//        if (img.empty()) {
//            std::cerr << "Error: Could not read the image " << file << std::endl;
//            continue;
//        }
//        
//        int height_ = img.rows;
//        int length_ = img.cols;
//        // Extract sub-images
//        cv::Mat new_image0 = img(cv::Range(0, height_ / 2), cv::Range(0, length_ / 2));
//        cv::Mat new_image45 = img(cv::Range(0, height_ / 2), cv::Range(length_ / 2, length_));
//        cv::Mat new_image90 = img(cv::Range(height_ / 2, height_), cv::Range(length_ / 2, length_));
//        cv::Mat new_image135 = img(cv::Range(height_ / 2, height_), cv::Range(0, length_ / 2));
//
//
//
//        // Save sub-images
//        //cout << file << endl;
//        size_t last_slash = file.find_last_of("\\/");
//        size_t last_dot = file.find_last_of('.');
//        //cout << last_dot << last_slash << endl;
//        //cout << file.substr(last_slash + 1, last_dot-last_slash-1) << endl;
//        cout << path + "\\0\\" + file.substr(last_slash + 1, last_dot - last_slash - 1) + "_0.png" << endl;
//        cv::imwrite(path + "\\0\\" + file.substr(last_slash + 1, last_dot - last_slash - 1) + "_0.png", new_image0);
//        cv::imwrite(path + "\\45\\" + file.substr(last_slash + 1, last_dot - last_slash - 1) + "_45.png", new_image45);
//        cv::imwrite(path + "\\90\\" + file.substr(last_slash + 1, last_dot - last_slash - 1) + "_90.png", new_image90);
//        cv::imwrite(path + "\\135\\" + file.substr(last_slash + 1, last_dot - last_slash - 1) + "_135.png", new_image135);
//    }
//
//    return 0;
//}
//
//
//
