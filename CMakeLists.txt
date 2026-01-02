# CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(BoundaryArtifactAnalysis)

# -----------------------------------------------------------------------------
# 1. 패키지 검색 (Geant4 + ROOT)
# -----------------------------------------------------------------------------
# Geant4 로드 (UI 및 시각화 포함)
find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

# ROOT 로드 (데이터 분석용)
find_package(ROOT REQUIRED)
include_directories(${ROOT_INCLUDE_DIRS})

# -----------------------------------------------------------------------------
# 2. 소스 코드 등록
# -----------------------------------------------------------------------------
include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

# -----------------------------------------------------------------------------
# 3. 실행 파일 생성 및 링크
# -----------------------------------------------------------------------------
add_executable(BoundaryTest main.cc ${sources} ${headers})
target_link_libraries(BoundaryTest ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

# -----------------------------------------------------------------------------
# 4. 편의성 설정 (매크로 및 스크립트 복사)
# -----------------------------------------------------------------------------
# 빌드 후 실행 파일이 있는 폴더로 run.mac과 분석 스크립트를 자동 복사합니다.
file(COPY run.mac DESTINATION ${CMAKE_BINARY_DIR})
# analysis 폴더가 있다면 함께 복사 (없으면 이 줄은 무시되거나 에러날 수 있으므로 확인 필요)
# file(COPY analysis/plot_artifact.py DESTINATION ${CMAKE_BINARY_DIR}) 

message(STATUS "Geant4 Version: ${Geant4_VERSION}")
message(STATUS "ROOT Version: ${ROOT_VERSION}")
