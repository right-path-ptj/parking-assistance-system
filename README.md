<img width="1156" height="622" alt="image" src="https://github.com/user-attachments/assets/28c1aefc-2141-44af-91d0-05c125da5b32" />

## 🛠️ Test-Driven Development (TDD) Approach

이 프로젝트는 임베디드 시스템의 안정성을 확보하기 위해 **TDD(Test-Driven Development)** 방법론을 일부 적용하여 개발되었습니다. 하드웨어 의존성이 높은 HAL 드라이버와 비즈니스 로직을 분리하여, 주요 알고리즘을 검증한 후 하드웨어에 통합하는 절차를 따랐습니다.

### 1. 개발 사이클 (Red-Green-Refactor)
* **Red (Test Fail):** 구현할 기능의 요구사항을 정의하고, 실패하는 테스트 케이스를 먼저 작성.
* **Green (Test Pass):** 기능을 통과시키기 위한 최소한의 코드 구현 (하드웨어 없이 로직 검증).
* **Refactor:** 코드의 중복을 제거하고 가독성을 높이며, 하드웨어(HAL)와 결합.

### 2. 주요 테스트 시나리오
임베디드 환경 특성상 전체 시스템 테스트 전, **단위 테스트(Unit Test)** 가능한 로직을 선별하여 검증했습니다.

#### A. 초음파 거리 계산 로직 검증 (`calc_diff`)
* **목표:** 타이머의 `Rising Edge`와 `Falling Edge` 캡처 값의 차이를 이용해 정확한 거리(cm)를 계산해야 함.
* **테스트 케이스:**
    1. `Fall > Rise` 인 일반적인 경우 (정상 계산 확인)
    2. `Rise > Fall` 인 타이머 오버플로우(Overflow) 발생 경우 (보정 로직 확인)
* **검증 결과:** 오버플로우 발생 시 `(0xFFFF - Rise) + Fall` 수식이 정상 동작함을 검증 후 적용.

#### B. 거리별 경보 시스템 로직 검증 (`update_alarm`)
* **목표:** 수신된 거리 데이터에 따라 PWM 주파수(ARR)와 Duty Cycle이 동적으로 변경되어야 함.
* **테스트 케이스:**
    1. **Input:** 90cm -> **Expect:** 경보 Off (PWM Duty 0)
    2. **Input:** 10cm (근접) -> **Expect:** 고주파 경보 (ARR 값 감소 확인)
    3. **Input:** 50cm (중간) -> **Expect:** 중간 주파수 경보
* **검증 결과:** 거리(`dist`)와 PWM 주기(`ARR`) 간의 선형 관계 수식 `(dist * 20) + 300` 도출 및 적용.

### 3. 통합 테스트 (Integration with CAN)
* **Loopback Test:** CAN 트랜시버 연결 전, MCU 내부 Loopback 모드를 활용하여 송신 데이터가 수신 버퍼에 정확히 들어오는지 검증.
* **Hardware Test:** 실제 센서와 액추에이터 보드를 분리하여 연결하고, 오실로스코프로 CAN H/L 파형을 분석하여 통신 무결성 확인.

can 통신을 이용하여 두개의 보드간 통신을 통해 각각 Actuator, Sensor 역할을 나누었습니다.

Actuator 보드- 부저모듈, led를 사용하여 sesor 보드로 받은 거리에 따라서 pwm 및 주기를 제어하여 가까울수록 경보를 울림.
Sensor 보드- 초음파 센서를 이용하여 거리를 측정함.

<img width="969" height="772" alt="image" src="https://github.com/user-attachments/assets/d0ac4666-e440-4efa-816b-ee2c4263bef9" />
<img width="564" height="504" alt="image" src="https://github.com/user-attachments/assets/75901517-0118-45d8-aa9f-3607483930cb" />





<img width="718" height="943" alt="image" src="https://github.com/user-attachments/assets/62bc84c5-b524-4ce6-85f6-af30ecdcdf74" />

<img width="717" height="849" alt="image" src="https://github.com/user-attachments/assets/b95ce9ed-3d56-4091-a987-1c10c8625f62" />


