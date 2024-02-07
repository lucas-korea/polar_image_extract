// �� �ڵ�� FLIR, LUCID ī�޶󿡼� ���� ���� �����͸�(.bin) �̹����� ��ȯ���ִ� �ڵ��̴�.
// 16bit ��� �����͸� �������� �ۼ��Ͽ���.
// �����̸��� ī�޶� �ø��� �ѹ��� �������� RGB, polar �����͸� ����
// python���� �ۼ��� ��, chatGPT�� �̿��� ��ȯ�Ǿ���, �Ϻ� �����Ͽ���

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

// python string�� endswith() �Լ��� �����Ͽ���. ���ڿ��� Ư�� ���̻�� ���������� Ȯ���ϴ� �Լ�
bool endsWith(std::string const& str, std::string const& suffix) {
    if (str.length() < suffix.length()) {
        return false;
    }
    return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

// RGB �̹����� ����
void createRGBColorImage(const std::vector<uint8_t>& dataSet, string outputfilename) {
    int width = 2448;
    int height = 2048;

    cv::Mat image(height, width, CV_16U);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            // 8bit�� �Էµ� dataset �����Ϳ� ����, 16bit �����ͷ� �ؼ��ϱ� ���� bit ���
            image.at<uint16_t>(i, j) = dataSet[(i * width + j) * 2] + 255 * dataSet[(i * width + j) * 2 + 1];
        }
    }

    Mat rotatedImage;
    rotate(image, rotatedImage, ROTATE_180); // ī�޶� �Ųٷ� �޷��־, �����ִ� �۾�

    Mat gray_image;
    rotatedImage.convertTo(gray_image, CV_16UC1);//ä�ο� ���� ������ �־��ֱ� ���� ������
    resize(rotatedImage, rotatedImage, Size(width / 4, height / 4)); // ����͸� �̹��� �۰� ������ֱ�, ���� ������� Ŀ�� ȭ�鿡 �� �Ⱥ���
    imshow("rotatedImage", rotatedImage); // �����ϴ� ���� ����͸���

    //resize(gray_image, gray_image, Size(width / 4, height / 4));
    //imshow("gray_image", gray_image);

    // bayer ������ �̹�����, color �̹����� �ٲ��ִ� ����.
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
    resize(RGB_image, RGB_image, Size(width / 4, height / 4)); // ����͸� �̹��� �۰� ������ֱ�, ���� ������� Ŀ�� ȭ�鿡 �� �Ⱥ���
    imshow("RGB_image", RGB_image); // �����ϴ� ���� ����͸���
    waitKey(1);

    cout << imagename + " RGB color image created" << endl;
}

// polar �̹����� ����
void createPolarSplitedColorImage(const std::vector<uint8_t>& dataSet, string path, string basename, string index) {
    int width = 2448;
    int height = 2048;

    cv::Mat image(height, width, CV_16U);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            // 8bit�� �Էµ� dataset �����Ϳ� ����, 16bit �����ͷ� �ؼ��ϱ� ���� bit ���
            image.at<uint16_t>(i, j) = dataSet[(i * width + j) * 2] + 255 * dataSet[(i * width + j) * 2 + 1];
        }
    }

    Mat rotatedImage;
    rotate(image, rotatedImage, ROTATE_180); // ī�޶� �Ųٷ� �޷��־, �����ִ� �۾�

    Mat gray_image;
    rotatedImage.convertTo(gray_image, CV_16UC1); //ä�ο� ���� ������ �־��ֱ� ���� ������

    // �� ���� �̹��� ����
    Mat image_0(height / 4, width / 4, CV_16UC3);
    Mat image_45(height / 4, width / 4, CV_16UC3);
    Mat image_135(height / 4, width / 4, CV_16UC3);
    Mat image_90(height / 4, width / 4, CV_16UC3);

    // ������ �����ϰ� �ִ� �ȼ� ������ ������ �־���. ������(������ ����) �� 90���̰�, �ð�������� 90, 45, 0 135�̴�.
    // ���� ī�޶� �Ųٷ� �޷� �־ �̹����� �Ųٷ� ������ �� �� �� ���� ���� 0�� �̰� �ð�������� 0,135,90,45 
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

    // ���� ���� 0�� �̰� �ð�������� 0,135,90,45 �ΰ��� �״�� ����, ������ �̹��� �ٿ��� all_angle �̹��� �����ϴ� ����
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

// python mkdirs() �Լ�. ���� ���� �Լ��̴�.
void mkdirs(string path, string subdirectory) {
    fs::path full_path = path + "\\" + subdirectory;
    if (!std::filesystem::exists(full_path)) {
        if (std::filesystem::create_directories(full_path)) {
            std::cout << full_path << "���丮 ���� ����" << std::endl;
        }
        else {
            std::cerr << full_path << "���丮 ���� ����" << std::endl;
        }
    }
    else {
        std::cout << "���丮 �̹� ������" << std::endl;
    }
}


int main() {
    string root_path = "E:\\240130_night"; // ī�޶� ��� binary �����Ͱ� �ִ� ���� 

    vector<string> file_list; // root_path�� ���� ��� list �����, python os.listdir() ���
    for (const auto& entry : fs::directory_iterator(root_path)) {
        string path{ entry.path().u8string() };
        cout << path << endl;
        file_list.push_back(path);
    }

    // file_list �� ���ϸ� ����Ͽ� polar ��������, RGB ������ list �����
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
    //vector<string> list_polar_slice(list_polar.begin() + 21, list_polar.end()); //�߰��� ��������� ���� �����̽����� ���
    for (string file : list_polar) {
        cout << cnt << " / " << list_polar.size() << " file" << endl;
        cnt++;
        string file_full_path = file;

        // �����̸��� ������_�ú���_�ø����ȣ�� ����� ����
        string base_name = file.substr(0, file.find_last_of("."));
        vector<std::string> tokens;
        string delimiter = "\\"; // �齽���÷� ���ڿ��� �����մϴ�.
        size_t pos = 0;
        while ((pos = base_name.find(delimiter)) != std::string::npos) {
            string token = base_name.substr(0, pos);
            tokens.push_back(token);
            base_name.erase(0, pos + delimiter.length());
        }


        // ifstream�� �̿��� ���� �б�, ���� ũ�� ����ϱ�,  
        // seekg�� å���� ����. ���� ũ�⸦ ����ϱ� �����̶� ������ ���ٰ� �ٽ� �����͸� ������� �б� ���� ó������ ���ƿ�
        ifstream input_file(file_full_path, ios::binary);
        input_file.seekg(0, ios::end); // ���� ������ �̵�
        streamsize file_size = input_file.tellg();
        cout << file_full_path << '\t' << file_size << endl;
        input_file.seekg(0, ios::beg); // ���� ó������ �̵�

        // �ػ󵵴� 2448*2048 ������ 16bit�̹���=2byte���� ���ϱ� 2�� ���־�� �Ѵ�. 
        int image_number = file_size / (2448 * 2048 * 2);
        mkdirs(root_path, "polar\\0");
        mkdirs(root_path, "polar\\45");
        mkdirs(root_path, "polar\\90");
        mkdirs(root_path, "polar\\135");
        mkdirs(root_path, "polar\\All_angle");
        for (int i = 0; i < image_number; ++i) {
            vector<unsigned char> data(2448 * 2048 * 2); 
            input_file.read((char*)data.data(), 2448 * 2048 * 2);

            if (i % 2 == 0) { //������ ���� ��������, Ư�������Ӹ� ������ �Ǵ�
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

    //    // �����̸��� ������_�ú���_�ø����ȣ�� ����� ����
    //    string base_name = file.substr(0, file.find_last_of("."));
    //    vector<std::string> tokens;
    //    string delimiter = "\\"; // �齽���÷� ���ڿ��� �����մϴ�.
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


    //    // ifstream�� �̿��� ���� �б�, ���� ũ�� ����ϱ�,  
    //    // seekg�� å���� ����. ���� ũ�⸦ ����ϱ� �����̶� ������ ���ٰ� �ٽ� �����͸� ������� �б� ���� ó������ ���ƿ�
    //    ifstream input_file(file_full_path, ios::binary);
    //    input_file.seekg(0, ios::end); // ���� ������ �̵�
    //    streamsize file_size = input_file.tellg();
    //    cout << file_full_path << '\t' << file_size << endl;
    //    input_file.seekg(0, ios::beg); // ���� ó������ �̵�

    //    // �ػ󵵴� 2448*2048 ������ 16bit�̹���=2byte���� ���ϱ� 2�� ���־�� �Ѵ�. 
    //    int image_number = file_size / (2448 * 2048 * 2);
    //    for (int i = 0; i < image_number; ++i) {
    //        vector<unsigned char> data(2448 * 2048 * 2);
    //        input_file.read((char*)data.data(), 2448 * 2048 * 2);
    //        if (i % 2 == 0) { //������ ���� ��������, Ư�������Ӹ� ������ �Ǵ�
    //            cout << i << " / " << image_number << " images" << endl;
    //            createRGBColorImage(data, outputFilename + "_" + to_string(i));
    //        }
    //    }
    //    input_file.close();
    //}

    return 0;
}