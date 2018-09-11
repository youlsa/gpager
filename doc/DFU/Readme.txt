# DFU 모드 진입 방법
1. GPAGER 와 PC 를 연결한다.
2. Wakeup 버튼을 LCD 화면에 'DFU Mode...' 가 표시 될때까지 누른다. (blacklight 는 켜지지 않는다)
3. dfu-util을 실행하여 펌웨어 업그레이드를 한다.


# DFU Util 사용법 (리눅스)
1. dfu-util.tar.gz 압축을 푼다. (tar zxvf dfu-util.tar.gz)
2. dfu-util/src/dfu-util 실행 파일을 확인
3. sudo ./dfu-util -D XXXX.dfu 를 실행하면 firmware 업그레이드가 진행된다. (관리자 권한으로 실행해야 함)

