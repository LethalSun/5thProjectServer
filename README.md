# 배틀 쉽  
유니티와 IOCP서버를 이용한 보드게임 배틀쉽 모작(온라인 플레이) 프로젝트  
[플레이 영상](https://youtu.be/b7AZi0Wd_eQ)  
[Client 레포지토리](https://github.com/LethalSun/UnitYK)  

---
## Game Server
사용언어: C++(IOCP)

입출력을 담당하는 IOCP 작업자 스레드와 로직스레드간의 동기화를 위해서 PPL의 Concurrency Queue
사용 했습니다. 
또 패킷을 보낼 때 IOCP 로직 스레드, 보내기를 계속 확인해 주는 스레드, 작업자 스레드가 각 
세션의 버퍼(두개의 버퍼를 번갈아 가면서 사용하는 개념적인 원형큐)와 그 상태를 나타내는 변수를
가지고 경쟁하기 때문에 상태 변수는 `std::Atomic<bool>`을 이용하고 버퍼는 Windows API의 
`CRITICAL_SECTION`을 이용해 동기화 했습니다. 
이 때 스핀 락을 이용하기위해 `InitializeCriticalSectionAndSpinCount` 함수를 이용해서 
초기화해 사용했습니다.  

서버가 동작하면 DB서버에 자신의 정보를 기록하고 현재 유저수를 갱신합니다.
## Login Server
사용언어: C#(ASP.NET)

클라이언트와 Http를 이용해서 통신을 합니다. DB서버에 유저정보와 게임서버 상태를 확인하고 유저가 존재한다면 클라이언트에게
게임서버 접속에 쓰일 토큰을 발행한후 토큰을 DB 서버에 기록합니다.

이 때 각각의 클라이언트는 정해진 게임서버에 등록되며 이 정보는 DB서버(Redis)에 기록됩니다. 
게임서버가 가득차면 새로운 게임서버를 켭니다.

## DB Server
사용언어: C#(ASP.NET)

게임서버, 로그인 서버와 Http를 이용해서 통신합니다. 유저의 고정적 정보를 저장하는 Mongo DB와 토큰같은
임시정보를 저장하는 Redis와 통신합니다.
