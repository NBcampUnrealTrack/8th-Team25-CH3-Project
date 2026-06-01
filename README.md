# AntarcticKids -- 자율주행 시뮬레이션

팀  | AntarcticKids (25조)

엔진 | Unreal Engine 5.5

========================================

프로젝트 개요

미래 도시를 배경으로 한 자율주행 자동차 브랜드의 시뮬레이션 프로그램.

실제 브랜드의 자율주행 시스템처럼 차량 상태를 실시간으로 시각화하고, 4가지 미션의 성공/실패로 자율주행 성능을 평가한다.

========================================

주요 특징

- 디지털 트윈 환경 |
  
  센서(LiDAR·Camera), 스플라인 AI, Chaos Vehicle을 통한 실제와 유사한 주행 환경 구성

- 사실적인 날씨 시스템 |

  비·눈·맑음의 자연스러운 전환 및 노면 습기·적설 머테리얼 실시간 반영

- 4가지 자율주행 미션 |

  신호등 준수 / 긴급 장애물 회피 / 속도 제한 구역 / 주차 성공 여부로 성능 평가

- 인게임 런타임 조작 |

  시뮬레이션 중 센서 설정·시간·날씨 변경 가능

- 실시간 데이터 로깅 |

  조향각·이동거리·가감속도 등 주행 데이터를 Excel 파일로 추출

========================================

팀원 및 역할

이정원 | Sensor, SensorVisualization, SubSystem, GameInstance, 상호작용 Actor 구현

김시리 | SplineFollowerComponent, AI Controller, BehaviorTree, 오프닝 UI 및 HUD 구현

윤서희 | 레벨 디자인, 월드맵 UI, 환경 아트, 에셋 통합 및 최적화

장원재 | Chaos Vehicle, 코드 통합, 설정 메뉴·로딩 화면·주행 기록 UI 구현

박건영 | DataLogger, 나이아가라 기반 Dynamic Weather System 구현

========================================

파일 구조

Source/AntarcitcKids/

├── Actor/        차량과 상호작용하는 환경 요소 (신호등, 장애물, 속도제한구역, 주차장) 및 날씨 Actor

├── AI/	     		  차량에 빙의하는 AI Controller

├── Camera/      	FreeCamera 등 카메라 Pawn (확장용)

├── Component/   	스플라인 추종 컴포넌트 (SplineFollowerComponent)

├── DataLogger/  	주행 데이터 기록 및 Excel 추출

├── Light/       	가로등·포인트 라이트 등 조명 시스템

├── Manager/     	씬 전반의 조명·초기 환경 설정을 중앙에서 관리

├── Quest/       	퀘스트 개별 상태(초기화→진행→완료) 및 타입별 데이터 정의

├── Road/        	스플라인 기반 커스텀 도로 Actor

├── Sensor/      	LiDAR·Camera 센서 및 BoundingBox 시각화

├── Subsystem/   	게임 전반 상태를 전역에서 제어하는 서브시스템 모음

├── System/      	PlayerController, GameMode 등 핵심 시스템

├── Vehicle/     	Chaos Vehicle 기반 차량 (Sedan 등)

└── Widget/      	일부 UI 위젯 코드 구현

========================================

실행 방법

1. UE 5.5에서 프로젝트 열기
2. 로비 레벨(L_Lobby)에서 시뮬레이션 시작
3. 인게임 단축키로 센서 뷰·LiDAR·카메라 전환 가능
