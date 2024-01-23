## 버전 정보 
- Opencv 1.4.3  
- Visual studio 2022  

## 구현 기능
 - LUCID 편광카메라로 취득된 이미지 추출 후 각도별 분리 저장 (2448*2048 bayer 구조) 
 - FLIR 카메라로 취득된 이미지 추출 (2448*2048 bayer 구조) 
 - 추출중인 이미지 실시간 모니터링 기능
 - 16bit로 취득된 데이터 추출 기능(16bit 이미지로 저장)

## 준비 및 동작
 1. opencv 연동  
 2. 카메라로 취득된 데이터가 있는 폴더 root_path로 설정  
 3. 실행. 실행 후 실시간 추출 중인 이미지 확인 및 저장 상태 확인 가능

## 함수 설명

#### createPolarSplitedColorImage 

```void createPolarSplitedColorImage(const std::vector<uint8_t>& dataSet, string path, string basename, string index)```
 - 편광카메라 로깅데이터에 대해, 이미지로 변환한 뒤 각도별로 추출하여 저장 (16 or 8 bit)
***
#### createRGBColorImage 

```void createRGBColorImage(const std::vector<uint8_t>& dataSet, string outputfilename)```
 - 편광카메라 로깅데이터에 대해, 이미지로 변환하여 저장 (16 or 8 bit)
***
#### endsWith 

```bool endsWith(std::string const& str, std::string const& suffix)```
 - python의 endswidth() 함수를 구현. string 인풋을 받아 str의 접미사가 특정 suffix로 끝나는지 판단
***
#### mkdirs 
```void mkdirs(string path, string subdirectory)```
 - python의 os.mkdirs()함수를 구현. path에 subdirectory 폴더를 생성

  

<center><img src="https://github.com/lucas-korea/adas_sensor_project/assets/57425658/83d8fac5-66cb-4305-89d6-d1cac55f406b" width="400" height="170"></center>  
<center>편광카메라 이미지 추출 장면</center>

<center><img src="https://github.com/lucas-korea/adas_sensor_project/assets/57425658/11e5e01b-a8fa-48bc-b3c2-0e96504616fc" width="100" height="70"></center>  
<center>편광카메라 이미지 추출 폴더 트리</center>

<center><img src="https://github.com/lucas-korea/adas_sensor_project/assets/57425658/d244756b-a705-4c6f-99a5-f24924e2bb34" width="400" height="170"></center>  
<center>편광카메라 이미지 추출 결과 예시</center>

