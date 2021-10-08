# 스마트 환경에서의 이상행위와 사이버 공격에 대한 모니터링 및 대응 솔루션 제작 (Server)

## Backend 
- 언어 : Cpp
- 개발환경 : WSL Ubuntu-20.04
- 개발도구 : Visual studio 2019 or Cmake

### Project Setting
- 플랫폼 : ARM 사용(32bit)
- 정적 라이브러리 셋팅
    
  cppcore, protocol 등 외부에서 만든 라이브러리를 WSL Ubuntu에 /usr/local/lib로 옮겨야한다.

  cf) visual studio 2019에서 리눅스 정적 라이브러리 연결 관련 오류가 존재한다고 한다.
    ```
    $ cp libprotocol.a /usr/local/lib
    $ cp libcppcore.a /usr/local/lib
    ```

- Visual Studio 2019 빌드 방법
    ```
    # 빌드 결과물
    PATH : Server/Build/ARM/Server.out
    ```

- Cmake 사용 시 빌드 방법
    ```
    # 빌드
    $ cd Server
    $ cmake Src
    $ cmake --build .

    # 빌드 결과물
    PATH : Server/Build/LinuxRelease/server
    ```

## Database
- 미정

### Database Setting
    ```

    ```

