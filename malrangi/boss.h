#pragma once
#include "client_api.h"

#pragma comment(lib,"winmm.lib")

class BossRaid
{
public:
	struct SKILL
	{
		BYTE Key;
		function<void(void)> Commande;

		enum
		{
			UNITARY = 0b00000001,
			RAPID = 0b00000010,
			BUF = 0b00000100,
			ASSIST = 0b00001000,
			ONOFF = 0b00010000,
			INSTALLATION = 0b00100000,
			ÉPHÉMÈRE = 0b01000000,
		};
		DWORD Flag;
		string NomDeFichier;
		milliseconds Delay;
		double Limite;

	private:
		void MettreDefaultDelay(DWORD Flag, milliseconds& Delay)
		{
			switch (Flag)
			{
			case UNITARY:
				Delay = milliseconds(800); break;
			case RAPID:
				Delay = milliseconds(400); break;
			case BUF:
			case ASSIST:
			case ONOFF:
			default:
				Delay = milliseconds(1000); break;
			}
		}

	public:
		SKILL(BYTE Key,
			DWORD Flag,
			const string& NomDeFichier,
			milliseconds Delay = milliseconds(-1),
			double Limite = 0.996) :
			Key(Key),
			Flag(Flag),
			NomDeFichier(NomDeFichier),
			Delay(Delay),
			Limite(Limite)
		{
			if (milliseconds(-1) == Delay)
			{
				MettreDefaultDelay(Flag, this->Delay);
			}
		}
		SKILL(BYTE Key,
			DWORD Flag,
			milliseconds Delay = milliseconds(-1)) :
			Key(Key),
			Flag(Flag),
			NomDeFichier(""),
			Delay(Delay)
		{
			if (milliseconds(-1) == Delay)
			{
				MettreDefaultDelay(Flag, this->Delay);
			}
		}
		SKILL(BYTE Key,
			function<void(void)> Commande,
			DWORD Flag,
			const string& NomDeFichier,
			milliseconds Delay = milliseconds(1000),
			double Limite = 0.996) :
			Key(Key),
			Commande(Commande),
			Flag(Flag),
			Delay(Delay),
			NomDeFichier(NomDeFichier),
			Limite(Limite)
		{
			if (milliseconds(-1) == Delay)
			{
				MettreDefaultDelay(Flag, this->Delay);
			}
		}
		SKILL(BYTE Key,
			function<void(void)> Commande,
			DWORD Flag,
			milliseconds Delay = milliseconds(1000),
			seconds TempsCharging = seconds(-1)) :
			Key(Key),
			Commande(Commande),
			Flag(Flag),
			Delay(Delay),
			NomDeFichier("")
		{
			if (milliseconds(-1) == Delay)
			{
				MettreDefaultDelay(Flag, this->Delay);
			}
		}
	};


public:
	map<USERCONF::CODE_PROPRE, vector<SKILL>> MapVecSkills;
	virtual void Transplanter(const USERCONF::CHARACTER_INFO& CharacterInfo) = 0;
	vector<SKILL> VecSkills;

public:
	SKILL LastSkill;
	int Reserved1;
	const USERCONF::KEYSET_INFO& KeysetInfo;

	inline static const Mat TargetImageItemPlusCoin = Read(TARGET_DIR "item//pluscoin.jpg");
	inline static const Mat TargetImageBossTimer = Read(TARGET_DIR "window//boss_timer.jpg");

public:
	BossRaid() :
		KeysetInfo(USERCONF::GetInstance()->VirtualKeyset),
		LastSkill(NULL, NULL)
	{
		MapVecSkills[USERCONF::CODE_PROPRE::닼나] =
		{
			{'1', SKILL::BUF, milliseconds(1200)},
			{'2', SKILL::BUF},
			{'3', SKILL::BUF},
			{'4', SKILL::BUF, "새크리파이스"},
			{VK_F2, SKILL::BUF, "다크_서스트"},
			{'T', SKILL::ASSIST, "에르다_노바"},
			{'R', SKILL::ASSIST, "피어스_사이클론"},
			{'E', SKILL::ASSIST, "다크_스피어"},
			{'W', SKILL::UNITARY},
			{'Q', SKILL::UNITARY},
		};
		MapVecSkills[USERCONF::CODE_PROPRE::썬콜] =
		{
			{'P', SKILL::ONOFF},
			{'1', SKILL::BUF},
			{'2', SKILL::BUF},
			{'3', SKILL::BUF},
			{'4', SKILL::BUF},
			{'5', SKILL::BUF},
			{VK_F2, SKILL::BUF, "인피니티"},
			{'T',
			[this]() {
				KeybdEventContinued('T', Reserved1);
			},
			SKILL::ASSIST, "프리징_브레스", milliseconds(600)},
			{'F', SKILL::INSTALLATION, "스피릿_오브_스노우"},
			{'W', SKILL::ASSIST, "프로즌_오브"},
			{'Z', SKILL::ASSIST, "아이스_에이지"},
			{'R', SKILL::ASSIST, "주피터_썬더"},
			{'D', SKILL::ASSIST, "썬더_브레이크"},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::비숍] =
		{
			{'L', SKILL::ONOFF},
			{'1', SKILL::BUF},
			{'2', SKILL::BUF},
			{'3', SKILL::BUF},
			{VK_F2, SKILL::BUF, "인피니티"},
			{'D', SKILL::BUF, "홀리_매직쉘"},
			{'R', SKILL::ASSIST, "헤븐즈_도어"},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::보마] =
		{
			{'O', SKILL::ONOFF, "익스트림_아쳐리_활"},
			{'2', SKILL::BUF},
			{'3', SKILL::BUF},
			{'F',
			[this]() {
				KeybdEventDown('F');
				Sleep(500);
				KeybdEvent(KeysetInfo.Talk);
				KeybdEventUp('F');
			},
			SKILL::BUF},
			{'Q', SKILL::RAPID}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::신궁] =
		{
			{'O', SKILL::ONOFF, "익스트림_아쳐리_석궁"},
			{'1', SKILL::BUF},
			{'2', SKILL::BUF},
			{'3', SKILL::BUF},
			{VK_F2, SKILL::BUF, "불스아이"},
			{VK_F3, SKILL::BUF, "스플릿_애로우"},
			{'T', SKILL::ASSIST, "에르다_노바"},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::패파] =
		{
			{'1', SKILL::BUF},
			{'2', SKILL::BUF},
			{'3', SKILL::BUF},
			{VK_F2, SKILL::BUF, "렐릭_에볼루션"},
			{'R', 
			[]()
			{
				KeybdEventContinued('R', 10000);
			},
			SKILL::ASSIST, "에이션트_아스트라"},
			{'W', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::나로] =
		{
			{'P', SKILL::ONOFF},
			{'1', SKILL::BUF},
			{'2', SKILL::BUF},
			{VK_F2, SKILL::BUF, "블리딩_톡신"},
			{VK_F3, SKILL::BUF, "스로우_블래스팅"},
			{VK_F4, SKILL::BUF, "스프레드_스로우"},
			{'T', SKILL::ASSIST, "에르다_노바"},
			{'R', SKILL::INSTALLATION, "다크로드의_비전서"},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::섀도어] =
		{
			{'1', SKILL::BUF},
			{'2', SKILL::BUF},
			{'3', SKILL::BUF},
			{'D', SKILL::INSTALLATION, "연막탄"},
			{'F', SKILL::INSTALLATION, "베일_오브_섀도우"},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::듀블] =
		{
			{'1', SKILL::BUF},
			{'2', SKILL::BUF},
			{'F', SKILL::BUF, "파이널_컷"},
			{VK_F2, SKILL::BUF, "히든_블레이드"},
			{VK_F3, 
			[](){
				KeybdEvent({VK_F3, VK_F3}, 1000);
			},
			SKILL::BUF, "레디_투_다이", milliseconds(0)},
			{'T', SKILL::ASSIST, "에르다_노바"},
			{'R', 
			[]()
			{
				KeybdEventContinued('R', 8000);
			},
			SKILL::ASSIST, "블레이드_스톰"},
			{'E', SKILL::ASSIST, "블레이드_토네이도"},
			{'Q', SKILL::UNITARY},
		};
		MapVecSkills[USERCONF::CODE_PROPRE::바이퍼] =
		{
			{'1', SKILL::BUF},
			{'2', SKILL::BUF},
			{'3', SKILL::BUF},
		//	{'L', SKILL::BUF, "스티뮬레이트"},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::캡틴] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::캐슈] =
		{
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::미하일] =
		{
			{'L', SKILL::BUF},
			{'W', SKILL::UNITARY},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::소마] =
		{
			{'1', SKILL::BUF},
			{'2', SKILL::BUF},
			{'3', SKILL::ONOFF, "폴링_문"},
			{'4', SKILL::BUF},
			{VK_F2, SKILL::BUF, "셀레스티얼_댄스"},
			{VK_F3, SKILL::BUF, "엘리시온"},
			{'T', SKILL::ASSIST, "소울_페네트레이션"},
			{'R', SKILL::ASSIST, "소울_이클립스"},
			{'E', SKILL::UNITARY},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::플위] =
		{
			{'1', SKILL::BUF},
			{'2', SKILL::BUF},
			{'3', SKILL::BUF},
			{'W', SKILL::ASSIST, "블레이징_익스팅션", milliseconds(800)},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::윈브] =
		{
			{'O', SKILL::ONOFF, "트라이플링_웜"},
			{'1', SKILL::BUF, milliseconds(1900)},
			{'2', SKILL::BUF, milliseconds(1500)},
			{VK_F2, SKILL::BUF, "윈드_월"},
			{'A', SKILL::ASSIST, "아이들_윔"},
			{'T', SKILL::ASSIST, "에르다_노바"},
			{'R', SKILL::ASSIST, "볼텍스_스피어"},
			{'E', SKILL::ASSIST, "하울링_게일", milliseconds(1000), 0.999},
			{'Q', SKILL::RAPID}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::나워] =
		{
			{'P', SKILL::ONOFF, "쉐도우_배트"},
			{'1', SKILL::BUF},
			{'2', SKILL::BUF},
			{'3', SKILL::BUF},
			{'5', SKILL::BUF},
			{VK_F2, SKILL::BUF, "쉐도우_스피어"},
			{VK_F3, SKILL::BUF, "쉐도우_일루전"},
			{'T', 
			[this]()
			{
				KeybdEventContinued('T', 1000);
			},
			SKILL::ASSIST, "쉐도우_스티치", milliseconds(500), 0.986},
			{'W', SKILL::ASSIST, "다크니스_오멘"},
			{'E', SKILL::ASSIST, "쉐도우_바이트"},
			{'R', 
			[]()
			{
				KeybdEventContinued('R', 2600);
			},
			SKILL::ASSIST, "래피드_스로우", milliseconds(200)},
			{'F', SKILL::INSTALLATION, "쉐도우_서번트_익스텐드"},
			{'Q', SKILL::UNITARY, milliseconds(600)}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::스커] =
		{
			{'2', SKILL::BUF, "축뢰"},
			{'3', SKILL::BUF},
			{'4', SKILL::BUF},
			{VK_F1, SKILL::BUF, "글로리_오브_가디언즈"},
			{VK_F2, SKILL::BUF, "신뇌합일"},
			{'T', SKILL::ASSIST, "에르다_노바"},
			{'Z', SKILL::ASSIST, "해신강림"},
			{'Q',
			[]() {
				KeybdEvent({'Q', 'W'}, 400);
			},
			SKILL::UNITARY, milliseconds(800)},
		};
		MapVecSkills[USERCONF::CODE_PROPRE::아란] =
		{
			{'3', SKILL::ONOFF},
			{'2', SKILL::BUF},
			{'L', SKILL::BUF, "아드레날린_제네레이터"},
			{'T', SKILL::ASSIST, "블리자드_템페스트", milliseconds(1200)},
			{'Z', SKILL::ASSIST, "마하의_영역", milliseconds(1400)},
			{'W',
			[]() {
				KeybdEvent({'W', 'E', 'E', 'E'}, 330);
				Sleep(120);
				KeybdEvent('E', 1000);
			},
			SKILL::UNITARY},
			{'0',
			[this]() {
				KeybdEvent({VK_DOWN, VK_RIGHT, VK_DOWN}, 150);
				KeybdEventContinued('Q', Reserved1);
			},
			SKILL::ÉPHÉMÈRE}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::에반] =
		{
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::메세] =
		{
			{'1', SKILL::BUF},
			{VK_F2, SKILL::BUF, "엘비쉬_블레싱"},
			{VK_F3, SKILL::BUF, "엘리멘탈_고스트"},
			{'T', SKILL::ASSIST, "에르다_노바"},
			{'R', 
			[]()
			{
				KeybdEventContinued('R', 6000);
			},
			SKILL::ASSIST, "이르칼라의_숨결", milliseconds(0)},
			{'Q', SKILL::RAPID}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::팬텀] =
		{
			{'O', SKILL::ONOFF},
			{'1', SKILL::BUF},
			{'2', SKILL::BUF, milliseconds(1200)},
			{'F', SKILL::BUF, "파이널_컷"},
			{VK_F2, SKILL::BUF, "다크_서스트"},
			{'T', SKILL::ASSIST, "에르다_노바"},
			{'R', 
			[]()
			{
				KeybdEventContinued('R', 6000);
			},
			SKILL::ASSIST, "조커"},
			{'W', SKILL::ASSIST, "블랙잭"},
			{'D', SKILL::ASSIST, "리프트_브레이크"},
			{'A', SKILL::RAPID}
		},
		MapVecSkills[USERCONF::CODE_PROPRE::루미] =
		{
			{'L', SKILL::BUF, "메모라이즈"},
			{'W', SKILL::INSTALLATION, "퍼니싱_리소네이터"},
			{'R', SKILL::INSTALLATION, "빛과_어둠의_세례"},
			{'S', SKILL::UNITARY},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::은월] =
		{
			{VK_F2, SKILL::INSTALLATION, "귀문진"},
			{VK_F3, SKILL::BUF, "정령_집속"},
			{VK_F4, SKILL::BUF, "정령_결속_극대화", milliseconds(1400)},
			{'T', SKILL::ASSIST, "속박술", milliseconds(1300)},
			{'E', SKILL::ASSIST, "파쇄_연권"},
			{'W', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::데벤] =
		{
			{'1', SKILL::BUF, milliseconds(1200)},
			{'2', SKILL::BUF},
			{'3', SKILL::BUF},
			{VK_F1, SKILL::BUF},
			{VK_F2, SKILL::BUF, "포비든_컨트랙트", milliseconds(1200)},
			{'E', SKILL::ASSIST, "실드_체이싱", milliseconds(750)},
			{'R', SKILL::ASSIST, "사우전드_소드"},
			{'T', SKILL::ASSIST, "블러디_임프리즌", milliseconds(1140)},
			{'W', SKILL::UNITARY},
			{'Q', SKILL::UNITARY, milliseconds(650)}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::데벤2] =
		{
			{'1', SKILL::BUF, milliseconds(1200)},
			{'2', SKILL::BUF},
			{'3', SKILL::BUF},
			{'4', SKILL::BUF},
			{VK_F1, SKILL::BUF},
			{VK_F2, SKILL::BUF, "포비든_컨트랙트", milliseconds(1200)},
			{VK_F3,
			[]()
			{
				KeybdEvent({ VK_F3, VK_F3 }, 580);
			},
			SKILL::ASSIST, "디멘션_소드", milliseconds(0)},
		//	{VK_F4, SKILL::BUF, "레버넌트"},
			{VK_F4, SKILL::BUF},
			{'E', SKILL::ASSIST, "실드_체이싱", milliseconds(750)},
			{'T', SKILL::ASSIST, "블러디_임프리즌", milliseconds(1140)},
			{'W', SKILL::UNITARY},
			{'Q', SKILL::UNITARY, milliseconds(650)}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::데슬] =
		{
			{'1', SKILL::BUF},
			{'2', SKILL::BUF, "메타모포시스", milliseconds(1800)},
			{'L', SKILL::BUF, "블루_블러드"},
			{VK_F2, SKILL::BUF, "오르트로스"},
			{VK_F4, SKILL::INSTALLATION, "요르문간드"},
			{'P', SKILL::BUF, "인피니티_포스"},
			{'R', 
			[]()
			{
				KeybdEventContinued('R', 1000);
			},
			SKILL::ASSIST, "데몬_베인"},
			{'T', SKILL::ASSIST, "다크_바인드", milliseconds(1200)},
			{'W', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::블래] =
		{
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::배메] =
		{
			{'L', SKILL::ONOFF, "배틀_레이지", milliseconds(1000), 0.993},
			{'P', SKILL::ONOFF},
			{'2', SKILL::ONOFF},
			{'1', SKILL::BUF},
			{VK_F2, SKILL::BUF, "마스터_오브_데스"},
			{VK_F3, SKILL::BUF, "유니온_오라"}, 
			{'D', SKILL::INSTALLATION, "그림_리퍼"}, 
			{'F', SKILL::INSTALLATION, "쉘터"},
			{'T', SKILL::ASSIST, "에르다_노바"},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::와헌] =
		{
			{'Q', SKILL::RAPID}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::제논] =
		{
			{'1', SKILL::BUF},
			{'2', SKILL::BUF},
			{'3', SKILL::BUF},
			{VK_F2, SKILL::BUF},
			{VK_F3, SKILL::BUF},
			{'T', SKILL::ASSIST, "컨파인_인탱글"},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::메카닉] =
		{
			{VK_F2, SKILL::BUF, "멀티플_옵션"},
			{VK_F3, SKILL::BUF, "메카_캐리어"},
			{VK_F4, SKILL::BUF, "봄버_타임"},
			{'W', SKILL::ASSIST, "디스토션_필드"},
			{'E', SKILL::ASSIST, "마이크로_미사일_컨테이너"},
			{'R',
			[]() {
				KeybdEventDown('R');
				Sleep(100);
				for (int p = 0; p < 44; p++)
				{
					KeybdEvent('D', 200);
				}
				Sleep(100);
				KeybdEventUp('R');
			},
			SKILL::ASSIST, "메탈아머_전탄발사"},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::카이저] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::엔버] =
		{
			{'1', SKILL::BUF},
			{'2', SKILL::BUF, milliseconds(1400)},
			{'O', SKILL::ONOFF},
			{VK_F2, SKILL::BUF, "소울_익절트"},
			{'E', SKILL::INSTALLATION, "슈퍼_노바"},
			{'F', 
			[]()
			{
				KeybdEventContinued('F', 8000);
			},
			SKILL::ASSIST, "소울_레조넌스", milliseconds(800)},
			{'W', SKILL::UNITARY},
		};
		MapVecSkills[USERCONF::CODE_PROPRE::제로] =
		{
			{'T', SKILL::ASSIST, "리미트_브레이크"},
			{'R', SKILL::ASSIST, "조인트_어택"},
			{'Q', 
			[]() {
				KeybdEvent({'Q', 'Q', 'Q', 'A', 'A', 'S', 'S', 'S'}, 800);
			},
			SKILL::ASSIST, milliseconds(800)}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::키네] =
		{
			{'Z', SKILL::BUF, "에버싸이킥", milliseconds(8000)},
			{'M', 
			[]()
			{
				KeybdEvent('Z', 0);
			},
			SKILL::ASSIST, "에버싸이킥", milliseconds(8000)},
			{'1', SKILL::BUF},
			{VK_F1, SKILL::BUF, "싸이킥_차징"},
			{VK_F2, SKILL::BUF, "싸이킥_오버"},
			{'E', SKILL::INSTALLATION, "무빙_매터"},
			{'R', SKILL::INSTALLATION, "싸이킥_토네이도"},
			{'F', SKILL::INSTALLATION, "싸이킥_그라운드2"},
			{'P', SKILL::ÉPHÉMÈRE},
			{'T', SKILL::ASSIST, "싸이코_메트리"},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::호영] =
		{
			{'1', SKILL::BUF},
			{'P',
			[]() {
				KeybdEvent('P', 3600);
				KeybdEvent({ '3', '2' }, 1000);
				Sleep(2000);
				KeybdEvent({ '4', '@' }, 1000);
			},
			SKILL::BUF, "영약_태을선단"},
			{'F', SKILL::ÉPHÉMÈRE},
			{'R',
			[]() {
				KeybdEvent({'R', 'R'}, 900);
			},
			SKILL::ASSIST, "강림_괴력난신", milliseconds(1800)},
			{'T', SKILL::ASSIST, "분신_둔갑_태을선인"},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::일리움] =
		{
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::아크] =
		{
			{'O', SKILL::ONOFF},
			{'1', SKILL::BUF},
			{'E',
			[]()
			{
				KeybdEventContinued('E', 3000);
			},
			SKILL::ASSIST, "끝없는_고통", milliseconds(800)},
			{'T', SKILL::ASSIST, "황홀한_구속"},
			{'R', SKILL::ASSIST, "근원의_기억"},
			{'Q', SKILL::UNITARY},
			{'A', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::카데나] =
		{
			{'1', SKILL::BUF},
			{'T', SKILL::ASSIST, "테이크다운"},
			{VK_F3, BossRaid::SKILL::INSTALLATION, "퓨리", milliseconds(1500)},
			{'W',
			[]() {
				KeybdEvent({'Q', 'W'}, 500);
			},
			SKILL::ASSIST, milliseconds(600)},
			{'A',
			[]() {
				KeybdEvent({'D', 'A', 'A', 'A'}, 700);
			},
			SKILL::ASSIST, "서먼_비팅_니들배트", milliseconds(500)},
			{'Q', SKILL::UNITARY }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::아델] =
		{
			{'5', SKILL::ONOFF},
			{'6', SKILL::ONOFF},
			{'R', SKILL::ASSIST, "인피니트"},
			{'T', SKILL::ASSIST, "스콜"},
			{'A', SKILL::ASSIST, "그레이브"},
			{'E', SKILL::ASSIST, "루인"},
			{'S', SKILL::ASSIST, "테리토리"},
			{'Q', SKILL::UNITARY}
		};

		for (int p = 0; p < (int)USERCONF::CODE_PROPRE::MAXIMUM; p++)
		{
			MapVecSkills[(USERCONF::CODE_PROPRE)p].push_back({ VK_OEM_3, SKILL::BUF });
		}
	}
	
public:
	void TransplanterStringÀVecSkills(const USERCONF::CODE_PROPRE Code, string StrSkills)
	{
		VecSkills.clear();
		for (BYTE c : StrSkills)
		{
			switch (c)
			{
			case '!':
				c = VK_F1; break;
			case '@':
				c = VK_F2; break;
			case '#':
				c = VK_F3; break;
			case '$':
				c = VK_F4; break;
			case '`':
				c = VK_OEM_3; break;
			}

			for (const auto s : MapVecSkills[Code])
			{
				if (c == s.Key)
				{
					VecSkills.push_back(s);
				}
			}
		}
	}
public:
	bool EstPremier;
	bool EstSkillDisponible(const SKILL& Skill)
	{
		switch (Skill.Flag)
		{
		case SKILL::ÉPHÉMÈRE:
		case SKILL::UNITARY:
		case SKILL::RAPID:
		case SKILL::ONOFF:
			return true;

		case SKILL::BUF:
		case SKILL::ASSIST:
		case SKILL::INSTALLATION:
		default:
			if ("" == Skill.NomDeFichier)
			{
				return true;
			}
			else
			{
				string a = string(TARGET_DIR) + "skill//" + Skill.NomDeFichier + ".png";

				Mat ImageSkill = Read(string(TARGET_DIR) + "skill//" + Skill.NomDeFichier + ".png", IMREAD_COLOR); // 32 x 32
				Mat sub = ImageSkill(Range(15, 30), Range(2, 18)); // y - x

				VALLOC mi;
				MatchTemplate(Capture({ 804, 714, 1363, 784 }, IMREAD_COLOR), sub, &mi, 0.9);

				cout << a << endl;
				cout << mi.Value << endl;
				return mi.Value > Skill.Limite;
			}
		}
	};
	bool EstSkillDisponible(const vector<SKILL>& VecSkills)
	{
		for (const auto& Skill : VecSkills)
		{
			if (!EstSkillDisponible(Skill))
			{
				return false;
			}
		}
		return true;
	}
	void AttendreJusqueDisponible(const vector<SKILL>& VecSkills)
	{
		auto TempsCommencé = system_clock::now();
		while (system_clock::now() - TempsCommencé < seconds(396))
		{
			if (EstSkillDisponible(VecSkills))
			{
				break;
			}
			Sleep(0x10);
		}
	}
	void UseSkills(int Flags)
	{
		auto UseSkill = [this](const SKILL& CurrentSkill)
		{
			if (EstSkillDisponible(CurrentSkill))
			{
				if (LastSkill.Key != NULL)
				{
					switch (CurrentSkill.Flag)
					{
					case SKILL::UNITARY:
					case SKILL::RAPID:
					case SKILL::ÉPHÉMÈRE:
						break;

					case SKILL::BUF:
					case SKILL::ASSIST:
					case SKILL::ONOFF:
					case SKILL::INSTALLATION:
					default:
						KeybdEventUp(LastSkill.Key);
						this_thread::sleep_for(LastSkill.Delay);
					}
				}

				if (CurrentSkill.Commande)
				{
					if (SKILL::ÉPHÉMÈRE == CurrentSkill.Flag)
					{
						if (EstPremier)
						{
							EstPremier = false;
							CurrentSkill.Commande();
						}
					}
					else
					{
						CurrentSkill.Commande();
					}
				}
				else
				{
					switch (CurrentSkill.Flag)
					{
					case SKILL::UNITARY:
					case SKILL::RAPID:
						KeybdEventDown(CurrentSkill.Key);
						break;

					case SKILL::ONOFF:
						if ("" != CurrentSkill.NomDeFichier)
						{
							Mat ImageSkill = Read(string(TARGET_DIR) + "skill//" + CurrentSkill.NomDeFichier + ".png"); // 32 x 32
							VALLOC mi;
							MatchTemplate(Capture({ 900, 25, 1363, 65 }), ImageSkill, &mi, 0.9);

							cout << string(TARGET_DIR) + "skill//" + CurrentSkill.NomDeFichier + ".png" << endl;
							cout << mi.Value << endl;

							if (mi.Value < CurrentSkill.Limite)
							{
								KeybdEvent(CurrentSkill.Key, 0);
							}
						}
						break;

					case SKILL::ÉPHÉMÈRE:
						if (EstPremier)
						{
							EstPremier = false;
						}
						else
						{
							break;
						}
					case SKILL::BUF:
					case SKILL::ASSIST:
					case SKILL::INSTALLATION:
					default:
						KeybdEvent(CurrentSkill.Key, 0);
					}
				}

				LastSkill = CurrentSkill;
			}
		};

		for (auto& Skill : VecSkills)
		{
			if (IS_FLAG_ON(Flags, Skill.Flag))
			{
				UseSkill(Skill);
				Sleep(1);
			}
		}
	}

	template <class T_LAMBDA2>
	void CombattreBoss(T_LAMBDA2 RoutineDeRevenir, seconds LimiteDeTemps, int Flags)
	{
		EstPremier = true;
		bool IsThreadJoinable = false;
		bool DevenezRéparer = false;
		auto ThreadResurrection = thread(
			[=, &IsThreadJoinable, &DevenezRéparer]()
			{
				static Mat ImageMort = Read(TARGET_DIR "pic//mort.png");
				while (!IsThreadJoinable)
				{
					if (MatchTemplate(SourceImageClient4, ImageMort))
					{
						DevenezRéparer = true;
						RoutineDeRevenir();
						DevenezRéparer = false;
					}
					Sleep(100);
				}
			}
		);
		auto ThreadBattle = thread(
			[&IsThreadJoinable, &DevenezRéparer, Flags, this]()
			{
				do
				{
					if (!DevenezRéparer)
					{
						UseSkills(Flags);
						Sleep(8);
					}
				} while (!IsThreadJoinable);
			}
		);

		bool IsRaidCompleteSuccessfully = 
			WaitUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageBossTimer, LimiteDeTemps);
		IsThreadJoinable = true;
		LastSkill.Key = NULL;

		if (ThreadBattle.joinable())
		{
			ThreadBattle.join();
		}
		if (ThreadResurrection.joinable())
		{
			ThreadResurrection.join();
		}

		for each (const auto & Skill in VecSkills)
		{
			KeybdEventUp(Skill.Key);
		}

		if (!IsRaidCompleteSuccessfully)
		{
			throw "레이딩 시간 초과";
		}
	}
};

class ZacumRaid : protected BossRaid
{
public:
	virtual void Transplanter(const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		string StrSkills;
		Reserved1 = 0;
		switch (CharacterInfo.Code)
		{
		case USERCONF::CODE_PROPRE::루미:
			StrSkills = "S"; break;
		case USERCONF::CODE_PROPRE::팬텀:
			StrSkills = "A"; break;
		case USERCONF::CODE_PROPRE::데벤:
		case USERCONF::CODE_PROPRE::데벤2:
			StrSkills = "EW"; break;
		case USERCONF::CODE_PROPRE::닼나:
		case USERCONF::CODE_PROPRE::패파:
		case USERCONF::CODE_PROPRE::미하일:
		case USERCONF::CODE_PROPRE::데슬:
		case USERCONF::CODE_PROPRE::엔버:
		case USERCONF::CODE_PROPRE::은월:
		case USERCONF::CODE_PROPRE::아란:
			StrSkills = "W"; break;
		case USERCONF::CODE_PROPRE::소마:
		case USERCONF::CODE_PROPRE::썬콜:
		case USERCONF::CODE_PROPRE::불독:
		case USERCONF::CODE_PROPRE::비숍:
		case USERCONF::CODE_PROPRE::보마:
		case USERCONF::CODE_PROPRE::신궁:
		case USERCONF::CODE_PROPRE::나로:
		case USERCONF::CODE_PROPRE::섀도어:
		case USERCONF::CODE_PROPRE::듀블:
		case USERCONF::CODE_PROPRE::바이퍼:
		case USERCONF::CODE_PROPRE::캐슈:
		case USERCONF::CODE_PROPRE::플위:
		case USERCONF::CODE_PROPRE::윈브:
		case USERCONF::CODE_PROPRE::나워:
		case USERCONF::CODE_PROPRE::스커:
		case USERCONF::CODE_PROPRE::에반:
		case USERCONF::CODE_PROPRE::메세:
		case USERCONF::CODE_PROPRE::블래:
		case USERCONF::CODE_PROPRE::배메:
		case USERCONF::CODE_PROPRE::와헌:
		case USERCONF::CODE_PROPRE::제논:
		case USERCONF::CODE_PROPRE::메카닉:
		case USERCONF::CODE_PROPRE::키네:
		case USERCONF::CODE_PROPRE::일리움:
		case USERCONF::CODE_PROPRE::카데나:
		case USERCONF::CODE_PROPRE::아크:
		case USERCONF::CODE_PROPRE::호영:
		case USERCONF::CODE_PROPRE::아델:
		case USERCONF::CODE_PROPRE::제로:
			StrSkills = "Q"; break;
		}
		TransplanterStringÀVecSkills(CharacterInfo.Code, StrSkills);
	}
	void Play(
		const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		static const Mat TargetImageEyeOfFire = Read(TARGET_DIR "item//eye_of_fire1.jpg");

		// Préparer à batailler
		MouseEvent(POS_VOID, 100);
		KeybdEventContinued(VK_RIGHT, 1200);
		UseSkills(SKILL::ONOFF | SKILL::BUF);
		Sleep(800);

		// Appeler le boss
		if (TRUE != ClientApi::JeteItem(TargetImageEyeOfFire))
		{
			throw "불의 눈 드롭 실패";
		}
		Sleep(3400);

		// Batailler
		CombattreBoss(
			[]() {
				Sleep(500);
				MouseEvent({ 800, 320 }, CLIC_GAUCHE, 200);
			},
			seconds(60),
				SKILL::UNITARY | SKILL::RAPID | SKILL::ASSIST | SKILL::ÉPHÉMÈRE);

		// Finir
		KeybdEventContinuedWithSubKey(VK_LEFT, KeysetInfo.Picking, 2400);
		KeybdEventContinuedWithSubKey(VK_RIGHT, KeysetInfo.Picking, 4800);
	}
};

class RootAbyssRaid : public BossRaid
{
protected:
	void AcheterÀPetrick(void)
	{
		static const Mat TargetImageNpcPetrick = Read(TARGET_DIR "npc//petrick.jpg");
		static const Mat ArrTargetImageItem1[] =
		{
			Read(TARGET_DIR "item//mastery_book20.jpg", IMREAD_COLOR),
			Read(TARGET_DIR "item//red_potion.jpg", IMREAD_COLOR),
			Read(TARGET_DIR "item//blue_potion.jpg", IMREAD_COLOR),
		//	Read(TARGET_DIR "item//reinforce.jpg", IMREAD_COLOR),
			Read(TARGET_DIR "item//mineral_pocket.jpg", IMREAD_COLOR),
			Read(TARGET_DIR "item//hub_pocket.jpg", IMREAD_COLOR)
		};
		static const Mat ArrTargetImageItem2[] =
		{
			Read(TARGET_DIR "item//crystal2.jpg", IMREAD_COLOR),
			Read(TARGET_DIR "item//crystal3.jpg", IMREAD_COLOR)
		};

		if (VALLOC MatchInfo;
			MatchTemplate(SourceImageClient4, TargetImageNpcPetrick, &MatchInfo))
		{
			MouseEvent(MatchInfo.Location, CLIC_GAUCHE);

			for (const auto& TargetImage : ArrTargetImageItem1)
			{
				if (MatchTemplate(Capture({ 0, 0, 600, 750 }, IMREAD_COLOR), TargetImage, &MatchInfo, 0.82))
				{
					MouseEvent(MatchInfo.Location, DOUBLECLIC_GAUCHE);
					KeybdEvent({ VK_RETURN, VK_RETURN }, 400);
				}
			}

			for (const auto& TargetImage : ArrTargetImageItem2)
			{
				if (MatchTemplate(Capture({ 0, 0, 600, 750 }, IMREAD_COLOR), TargetImage, &MatchInfo))
				{
					MouseEvent(MatchInfo.Location, DOUBLECLIC_GAUCHE);
					KeybdEvent({ '3', '0', VK_RETURN, VK_RETURN }, 400);
				}
			}

			MouseEvent({ 660, 220 }, CLIC_GAUCHE);
			MouseEvent(POS_VOID, CLIC_GAUCHE);
		}
	}
};

class RootAbyssBloodyQueenRaid : public RootAbyssRaid
{
public:
	virtual void Transplanter(const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		string StrSkills;
		Reserved1 = 0;
		switch (CharacterInfo.Code)
		{
		case USERCONF::CODE_PROPRE::닼나:
			StrSkills = "1EW"; break;
		case USERCONF::CODE_PROPRE::불독:
			StrSkills = "RWQ"; break;
		case USERCONF::CODE_PROPRE::썬콜:
			StrSkills = "RWQ"; break;
		case USERCONF::CODE_PROPRE::비숍:
			StrSkills = "DRQ@"; break;
		case USERCONF::CODE_PROPRE::보마:
			StrSkills = "OWQ"; break;
		case USERCONF::CODE_PROPRE::신궁:
			StrSkills = "OQ"; break;
		case USERCONF::CODE_PROPRE::패파:
			StrSkills = "W"; break;
		case USERCONF::CODE_PROPRE::섀도어:
			StrSkills = "123FQ"; break;
		case USERCONF::CODE_PROPRE::나로:
			StrSkills = "2RQ"; break;
		case USERCONF::CODE_PROPRE::듀블:
			StrSkills = "2EQ"; break;
		case USERCONF::CODE_PROPRE::바이퍼:
			StrSkills = "123LQ"; break;
		case USERCONF::CODE_PROPRE::미하일:
			StrSkills = "LQ"; break;
		case USERCONF::CODE_PROPRE::소마:
			StrSkills = "4Q"; break;
		case USERCONF::CODE_PROPRE::플위:
			StrSkills = "2DQ"; break;
		case USERCONF::CODE_PROPRE::윈브:
			StrSkills = "O12Q"; break;
		case USERCONF::CODE_PROPRE::나워:
			StrSkills = "P1EWFQ"; break;
		case USERCONF::CODE_PROPRE::스커:
			StrSkills = "Q"; break;
		case USERCONF::CODE_PROPRE::아란:
			StrSkills = "RW"; break;
		case USERCONF::CODE_PROPRE::메세:
			StrSkills = "1Q"; break;
		case USERCONF::CODE_PROPRE::팬텀:
			StrSkills = "WDA"; break;
		case USERCONF::CODE_PROPRE::루미:
			StrSkills = "S"; break;
		case USERCONF::CODE_PROPRE::은월:
			StrSkills = "W"; break;
		case USERCONF::CODE_PROPRE::데슬:
			StrSkills = "W"; break;
		case USERCONF::CODE_PROPRE::데벤:
		case USERCONF::CODE_PROPRE::데벤2:
			StrSkills = "123EQ"; break;
		case USERCONF::CODE_PROPRE::블래:
			StrSkills = "Q"; break;
		case USERCONF::CODE_PROPRE::배메:
			StrSkills = "L2Q"; break;
		case USERCONF::CODE_PROPRE::제논:
			StrSkills = "23Q"; break;
		case USERCONF::CODE_PROPRE::메카닉:
			StrSkills = "WEQ"; break;
		case USERCONF::CODE_PROPRE::엔버:
			StrSkills = "O2EAFW"; break;
		case USERCONF::CODE_PROPRE::제로:
			StrSkills = "Q"; break;
		case USERCONF::CODE_PROPRE::키네:
			StrSkills = "PQ"; break;
		case USERCONF::CODE_PROPRE::카데나:
			StrSkills = "WAQ"; break;
		case USERCONF::CODE_PROPRE::호영:
			StrSkills = "Q"; break;
		case USERCONF::CODE_PROPRE::아크:
			StrSkills = "FA"; break;
		case USERCONF::CODE_PROPRE::아델:
			StrSkills = "SQ"; break;
		}
		TransplanterStringÀVecSkills(CharacterInfo.Code, StrSkills);
	}
	void Play(
		const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		// Préparer à batailler
		KeybdEventContinued(VK_RIGHT, GET_DURATION(240000, CharacterInfo.Vitesse));
		AttendreJusqueDisponible(VecSkills);
		UseSkills(SKILL::BUF | SKILL::ONOFF | SKILL::INSTALLATION | SKILL::ÉPHÉMÈRE);
		Sleep(500);

		// Appeler le boss
		for (int i = 0; i < 10; i++)
		{
			MouseEvent({ 870, 561 }, CLIC_GAUCHE, 290);
		}

		// Batailler
		CombattreBoss(
			[]() {
				Sleep(4000);
				MouseEvent({ 800, 320 }, CLIC_GAUCHE, 200);
			},
			seconds(120),
			SKILL::UNITARY | SKILL::RAPID | SKILL::ASSIST | SKILL::ÉPHÉMÈRE);
		Sleep(600);

		// Finir
		switch (CharacterInfo.Code)
		{
		case USERCONF::CODE_PROPRE::데벤:
		case USERCONF::CODE_PROPRE::데벤2:
			Sleep(3800);
			break;

		case USERCONF::CODE_PROPRE::제로:
			KeybdEvent('X', 1200);
		default:
			KeybdEventContinued(VK_LEFT, 8000);
			KeybdEventContinued(VK_RIGHT, GET_DURATION(648000, CharacterInfo.Vitesse));
			Sleep(500);
			break;
		}

		AcheterÀPetrick();

		switch (CharacterInfo.Code)
		{
		default:
			KeybdEvent(KeysetInfo.Attack);
			break;

		case USERCONF::CODE_PROPRE::데슬:
		case USERCONF::CODE_PROPRE::아란:
		case USERCONF::CODE_PROPRE::카데나:
			KeybdEvent('Q');
			break;

		case USERCONF::CODE_PROPRE::엔버:
			KeybdEventContinued(KeysetInfo.Attack, 500);
			break;

		case USERCONF::CODE_PROPRE::제로:
			for (int q = 0; q < 4; q++)
			{
				KeybdEvent({ KeysetInfo.Attack }, 800);
			}
			break;
		}

		KeybdEventContinuedWithSubKey(VK_RIGHT, KeysetInfo.Picking, 1500);
		KeybdEventContinuedWithSubKey(VK_LEFT, KeysetInfo.Picking, 3000);
	}
};

class RootAbyssVonBanRaid : public RootAbyssRaid
{
protected:
	virtual void Transplanter(const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		string StrSkills;
		Reserved1 = 0;
		switch (CharacterInfo.Code)
		{
		case USERCONF::CODE_PROPRE::닼나:
			StrSkills = "1REW"; break;
		case USERCONF::CODE_PROPRE::불독:
			StrSkills = "RWQ"; break;
		case USERCONF::CODE_PROPRE::썬콜:
			Reserved1 = 2000;
			StrSkills = "23@TRFZDWQ"; break;
		case USERCONF::CODE_PROPRE::비숍:
			StrSkills = "DRQ@"; break;
		case USERCONF::CODE_PROPRE::신궁:
			StrSkills = "O123!@$Q"; break;
		case USERCONF::CODE_PROPRE::패파:
			StrSkills = "3RW@"; break;
		case USERCONF::CODE_PROPRE::섀도어:
			StrSkills = "123DFQ"; break;
		case USERCONF::CODE_PROPRE::나로:
			StrSkills = "P2RQ@#$"; break;
		case USERCONF::CODE_PROPRE::듀블:
			StrSkills = "1FREQF@#"; break;
		case USERCONF::CODE_PROPRE::소마:
			StrSkills = "TR@Q"; break;
		case USERCONF::CODE_PROPRE::윈브:
			StrSkills = "O12@REAQ"; break;
		case USERCONF::CODE_PROPRE::나워:
			StrSkills = "P1TEWFRQ"; break;
		case USERCONF::CODE_PROPRE::스커:
			StrSkills = "Q"; break;
		case USERCONF::CODE_PROPRE::아란:
			StrSkills = "TRW"; break;
		case USERCONF::CODE_PROPRE::메세:
			StrSkills = "1RQ"; break;
		case USERCONF::CODE_PROPRE::팬텀:
			StrSkills = "12RDW"; break;
		case USERCONF::CODE_PROPRE::루미:
			StrSkills = "S"; break;
		case USERCONF::CODE_PROPRE::은월:
			StrSkills = "#TW"; break;
		case USERCONF::CODE_PROPRE::데벤:
		case USERCONF::CODE_PROPRE::데벤2:
			StrSkills = "123!@TE#Q"; break;
		case USERCONF::CODE_PROPRE::데슬:
			StrSkills = "2$TLW"; break;
		case USERCONF::CODE_PROPRE::블래:
			StrSkills = "Q"; break;
		case USERCONF::CODE_PROPRE::배메:
			StrSkills = "LP12DQ@#"; break;
		case USERCONF::CODE_PROPRE::제논:
			StrSkills = "2@3TQ"; break;
		case USERCONF::CODE_PROPRE::메카닉:
			StrSkills = "@#$RQ"; break;
		case USERCONF::CODE_PROPRE::엔버:
			StrSkills = "2EAFD"; break;
		case USERCONF::CODE_PROPRE::제로:
			StrSkills = "RQ"; break;
		case USERCONF::CODE_PROPRE::키네:
			StrSkills = "TPQ"; break;
		case USERCONF::CODE_PROPRE::호영:
			StrSkills = "PFQ"; break;
		case USERCONF::CODE_PROPRE::카데나:
			StrSkills = "1#TQ"; break;
		case USERCONF::CODE_PROPRE::아크:
			StrSkills = "RQ"; break;
		case USERCONF::CODE_PROPRE::아델:
			StrSkills = "RESQ"; break;
		}
		TransplanterStringÀVecSkills(CharacterInfo.Code, StrSkills);
	}
public:
	void Play(
		const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		// Préparer à batailler
		AttendreJusqueDisponible(VecSkills);
		UseSkills(SKILL::BUF | SKILL::ONOFF | SKILL::INSTALLATION);

		// Appeler le boss
		MouseEvent({ 995, 570 }, CLIC_GAUCHE, 0);

		// Batailler
		switch (CharacterInfo.Code)
		{
		default:
			Sleep(2100);
			break;
		case USERCONF::CODE_PROPRE::썬콜:
		case USERCONF::CODE_PROPRE::메카닉:
		case USERCONF::CODE_PROPRE::팬텀:
		case USERCONF::CODE_PROPRE::메세:
		case USERCONF::CODE_PROPRE::나워:
		case USERCONF::CODE_PROPRE::듀블:
		case USERCONF::CODE_PROPRE::패파:
		case USERCONF::CODE_PROPRE::나로:
		case USERCONF::CODE_PROPRE::윈브:
			Sleep(800);
			break;
		}

		CombattreBoss(
			[&CharacterInfo]() {
				Sleep(2000);
				MouseEvent({ 800, 320 }, CLIC_GAUCHE, 800);

				switch (CharacterInfo.Code)
				{
				default:
					break;

				case USERCONF::CODE_PROPRE::아크:
					KeybdEvent('Z', 800);
					break;
				}
			},
			seconds(120),
				SKILL::UNITARY | SKILL::RAPID | SKILL::ASSIST | SKILL::ÉPHÉMÈRE);

		// Finir
		Sleep(1000);
		switch (CharacterInfo.Code)
		{
		default:
			break;

		case USERCONF::CODE_PROPRE::제로:
			KeybdEvent('X', 1200);
			break;

		case USERCONF::CODE_PROPRE::아크:
			KeybdEvent({ 'X', 'Z' }, 1400);
			break;
		}

		KeybdEventContinuedWithSubKey(VK_LEFT, KeysetInfo.Picking, 3300);
		KeybdEventContinuedWithSubKey(VK_RIGHT, KeysetInfo.Picking, 7000);
		ClientApi::DiminuerChatWindow();
		AcheterÀPetrick();
	}
};

class BellumRaid : public RootAbyssRaid
{
protected:
	virtual void Transplanter(const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		string StrSkills;
		Reserved1 = 0;
		switch (CharacterInfo.Code)
		{
		case USERCONF::CODE_PROPRE::닼나:
			StrSkills = "`T1234!@EQR"; break;
		case USERCONF::CODE_PROPRE::썬콜:
			Reserved1 = 4000;
			StrSkills = "`TR23@RFZDWQ"; break;
		case USERCONF::CODE_PROPRE::신궁:
			StrSkills = "`TO123!@#Q"; break;
		case USERCONF::CODE_PROPRE::나로:
			StrSkills = "`T12!@#$RQ"; break;
		case USERCONF::CODE_PROPRE::듀블:
			StrSkills = "`T1!@F#ERQ"; break;
		case USERCONF::CODE_PROPRE::소마:
			StrSkills = "`TR#E";break;
		case USERCONF::CODE_PROPRE::윈브:
			StrSkills = "O`12@TREAQ"; break;
		case USERCONF::CODE_PROPRE::나워:
			StrSkills = "P`1235!@#TEWFRQ"; break;
		case USERCONF::CODE_PROPRE::스커:
			StrSkills = "ZQ"; break;
		case USERCONF::CODE_PROPRE::아란:
			Reserved1 = 1000;
			StrSkills = "2L0W"; break;
		case USERCONF::CODE_PROPRE::메세:
			StrSkills = "`T1!@#RQ"; break;
		case USERCONF::CODE_PROPRE::팬텀:
			StrSkills = "`T123FWRDA"; break;
		case USERCONF::CODE_PROPRE::루미:
			StrSkills = "RS"; break;
		case USERCONF::CODE_PROPRE::은월:
			StrSkills = "`T!@#$EW"; break;
		case USERCONF::CODE_PROPRE::데슬:
			StrSkills = "`T12L@$W"; break;
		case USERCONF::CODE_PROPRE::데벤:
		case USERCONF::CODE_PROPRE::데벤2:
			StrSkills = "T1234!@$#EQ"; break;
		case USERCONF::CODE_PROPRE::배메:
			StrSkills = "`TL12!D@#Q"; break;
		case USERCONF::CODE_PROPRE::메카닉:
			StrSkills = "`!@#$REWQ"; break;
		case USERCONF::CODE_PROPRE::제로:
			StrSkills = "`TRQ"; break;
		case USERCONF::CODE_PROPRE::키네:
			StrSkills = "`1Z@TERFQ"; break;
		case USERCONF::CODE_PROPRE::아크:
			StrSkills = "`TRQ"; break;
		case USERCONF::CODE_PROPRE::아델:
			StrSkills = "`TRAESQ"; break;
		case USERCONF::CODE_PROPRE::호영:
			StrSkills = "`T1P@FRQ"; break;
		}
		TransplanterStringÀVecSkills(CharacterInfo.Code, StrSkills);
	}
public:
	void Play(
		const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		AttendreJusqueDisponible(VecSkills);
		UseSkills(SKILL::BUF | SKILL::ONOFF | SKILL::INSTALLATION);
		Sleep(200);
		MouseEvent({ 1220, 450 }, CLIC_GAUCHE, 5000);

		// 벨룸 튀어나오는 시간 : 5.5s ~ 7s
		// 벨룸이 머무는 시간 : 2s

		auto OùEstBellum = []()
		{
			Mat Mask;
			if (InRange(Capture({ 750, 600, 1368, 660 }), Scalar(214, 176, 37), Scalar(239, 223, 104), Mask))
			{
				for (int y = 0; y < Mask.rows; ++y)
				{
					for (int x = 0; x < Mask.cols; ++x)
					{
						if (Mask.at<BYTE>(y, x) == 255)
						{
							return x;
						}
					}
				}
			}
			return -1;
		};

		int x;
		while ((x = OùEstBellum()) == -1)
		{
			Sleep(1);
		}
		// 벨룸 튀어나오기 직전
		//cout << x << endl;
		
		switch (CharacterInfo.Code)
		{
		default: 
		{
			int c = 0; // 0 - 긴급이동 요 / 1 - 캐릭터 직전 소환 / 2 - 제자리소환 
			if (x >= 0 && x <= 150)
			{
				// 긴급이동 : 800 ms 소요
				switch (CharacterInfo.Code)
				{
				default:
					KeybdEventContinued(VK_LEFT, 50);
					KeybdEvent(VK_MENU, 50);
					KeybdEvent(VK_MENU, 50);
					KeybdEvent(VK_MENU, 600);
					KeybdEventContinued(VK_RIGHT, 50);
					break;
				case USERCONF::CODE_PROPRE::썬콜:
				case USERCONF::CODE_PROPRE::비숍:
					KeybdEventDown(VK_LEFT);
					KeybdEvent('C', 750);
					KeybdEventUp(VK_RIGHT);
					KeybdEventContinued(VK_RIGHT, 50);
					break;

				case USERCONF::CODE_PROPRE::은월:
					KeybdEvent('X', 800);
					break;
				}
			}
			else if (x > 150 && x < 300)
			{
				c = 1;
			}
			else
			{
				c = 2;
			}

			switch (CharacterInfo.Code)
			{
			default: // 에르다노바류
				for (int i = 0; i < 10; i++)
				{
					KeybdEvent('T', 100);
				}
				break;

				// 일반 바인드류
			case USERCONF::CODE_PROPRE::아델:
			case USERCONF::CODE_PROPRE::데벤:
			case USERCONF::CODE_PROPRE::데벤2:
			case USERCONF::CODE_PROPRE::데슬:
			case USERCONF::CODE_PROPRE::제로:
				Sleep(1000);
				break;

				// 일반 바인드류 (딜레이)
			case USERCONF::CODE_PROPRE::호영:
				Sleep(200);
				break;

				// 차징 바인드류
			case USERCONF::CODE_PROPRE::나워:
				break;

				// 근접 바인드류
			case USERCONF::CODE_PROPRE::은월:
			case USERCONF::CODE_PROPRE::아크:
			case USERCONF::CODE_PROPRE::소마:
			case USERCONF::CODE_PROPRE::키네:
				if (0 == c)
				{
					KeybdEventContinued(VK_RIGHT, 200);
				}
				else if (1 == c)
				{
					Sleep(1000);
				}
				else if (2 == c)
				{
					KeybdEventContinued(VK_RIGHT, 500);
					Sleep(500);
				}
				break;
			}
			break;
		}

		//무적기류
		case USERCONF::CODE_PROPRE::아란:
		case USERCONF::CODE_PROPRE::스커:
		case USERCONF::CODE_PROPRE::루미:
		case USERCONF::CODE_PROPRE::썬콜:
		case USERCONF::CODE_PROPRE::메카닉:
			break;
		}
	
		CombattreBoss(
			[&CharacterInfo]() {
				Sleep(2000);
				MouseEvent({ 800, 320 }, CLIC_GAUCHE, 800);

				switch (CharacterInfo.Code)
				{
				default:
					break;

				case USERCONF::CODE_PROPRE::아크:
					KeybdEvent('Z', 800);
					break;
				}
			},
			seconds(50),
				SKILL::UNITARY | SKILL::RAPID | SKILL::ASSIST | SKILL::ÉPHÉMÈRE);

		// Finir
		Sleep(1000);
		switch (CharacterInfo.Code)
		{
		default:
			break;

		case USERCONF::CODE_PROPRE::제로:
			KeybdEvent('X', 1200);
			break;

		case USERCONF::CODE_PROPRE::아크:
			KeybdEvent({ 'X', 'Z' }, 1400);
			break;
		}

		KeybdEventContinuedWithSubKey(VK_LEFT, KeysetInfo.Picking, 6000);
		KeybdEventContinuedWithSubKey(VK_RIGHT, KeysetInfo.Picking, 9000);
		ClientApi::DiminuerChatWindow();
		AcheterÀPetrick();
	}
};

class VonLeonRaid : protected BossRaid
{
protected:
	virtual void Transplanter(const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		string StrSkills;
		switch (CharacterInfo.Code)
		{
		case USERCONF::CODE_PROPRE::닼나:
			StrSkills = "1234EQ"; break;
		case USERCONF::CODE_PROPRE::썬콜:
			StrSkills = "RZDWQ"; break;
		case USERCONF::CODE_PROPRE::보마:
			StrSkills = "O23FQ"; break;
		case USERCONF::CODE_PROPRE::신궁:
			StrSkills = "O123Q"; break;
		case USERCONF::CODE_PROPRE::패파:
			StrSkills = "3RW@"; break;
		case USERCONF::CODE_PROPRE::섀도어:
			StrSkills = "123FQ"; break;
		case USERCONF::CODE_PROPRE::나로:
			StrSkills = "12RQ"; break;
		case USERCONF::CODE_PROPRE::듀블:
			StrSkills = "12EQ"; break;
		case USERCONF::CODE_PROPRE::소마:
			StrSkills = "124Q"; break;
		case USERCONF::CODE_PROPRE::윈브:
			StrSkills = "O`12REAQ"; break;
		case USERCONF::CODE_PROPRE::나워:
			StrSkills = "P1EWFQ"; break;
		case USERCONF::CODE_PROPRE::스커:
			StrSkills = "234!@TZQ"; break;
		case USERCONF::CODE_PROPRE::아란:
			StrSkills = "RW"; break;
		case USERCONF::CODE_PROPRE::메세:
			StrSkills = "1Q"; break;
		case USERCONF::CODE_PROPRE::팬텀:
			StrSkills = "12@WDA"; break;
		case USERCONF::CODE_PROPRE::루미:
			StrSkills = "RS"; break;
		case USERCONF::CODE_PROPRE::은월:
			StrSkills = "W"; break;
		case USERCONF::CODE_PROPRE::데슬:
			StrSkills = "1W"; break;
		case USERCONF::CODE_PROPRE::데벤:
		case USERCONF::CODE_PROPRE::데벤2:
			StrSkills = "123EQ"; break;
		case USERCONF::CODE_PROPRE::배메:
			StrSkills = "L12Q"; break;
		case USERCONF::CODE_PROPRE::제논:
			StrSkills = "12@3Q"; break;
		case USERCONF::CODE_PROPRE::메카닉:
			StrSkills = "WEQ"; break;
		case USERCONF::CODE_PROPRE::제로:
			StrSkills = "Q"; break;
		case USERCONF::CODE_PROPRE::엔버:
			StrSkills = "XO2@EAFW"; break;
		case USERCONF::CODE_PROPRE::키네:
			StrSkills = "PFQ"; break;
		case USERCONF::CODE_PROPRE::호영:
			StrSkills = "1Q"; break;
		case USERCONF::CODE_PROPRE::카데나:
			StrSkills = "1WAQ"; break;
		case USERCONF::CODE_PROPRE::아크:
			StrSkills = "EQ"; break;
		case USERCONF::CODE_PROPRE::아델:
			StrSkills = "ESQ"; break;
		}
		TransplanterStringÀVecSkills(CharacterInfo.Code, StrSkills);
	}
public:
	void Play(
		const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		// Préparer à batailler
		MinimapRecognizer Recognizer({ 10, 80, 260, 150 });
		Recognizer.BougerÀCritèreRelatif(140);

		KeybdEvent(KeysetInfo.Potion);
		switch (CharacterInfo.Code)
		{
		case USERCONF::CODE_PROPRE::패파:
			KeybdEvent('Q', 1200);
			break;
		}
		AttendreJusqueDisponible(VecSkills);
		UseSkills(SKILL::BUF | SKILL::ONOFF);
		Sleep(1200);

		// Appeler le boss
		KeybdEvent({ KeysetInfo.Talk, VK_RIGHT, VK_RETURN });
		if (CharacterInfo.Stance)
		{
			Sleep(7000);
		}
		else
		{
			KeybdEventContinued(VK_LEFT, 1200);
			KeybdEventContinued(VK_RIGHT, 200);
			Sleep(5800);
		}

		// Batailler
		CombattreBoss(
			[&CharacterInfo]() {
				Sleep(500);
				MouseEvent({ 800, 320 }, CLIC_GAUCHE, 800);

				switch (CharacterInfo.Code)
				{
				default:
					break;

				case USERCONF::CODE_PROPRE::아크:
					KeybdEvent('Z', 800);
					break;
				}

				KeybdEventContinued(VK_RIGHT, GET_DURATION(1220000, CharacterInfo.Vitesse));
				KeybdEventContinued(VK_LEFT, 200);
			},
			seconds(120),
				SKILL::UNITARY | SKILL::RAPID | SKILL::ASSIST | SKILL::INSTALLATION | SKILL::ÉPHÉMÈRE);

		// Finir
		Sleep(1000);
		switch (CharacterInfo.Code)
		{
		default:
			break;

		case USERCONF::CODE_PROPRE::제로:
			KeybdEvent('X', 1200);
			break;

		case USERCONF::CODE_PROPRE::아크:
			KeybdEvent({ 'X', 'Z' }, 1400);
			break;
		}
		KeybdEventContinuedWithSubKey(VK_LEFT, KeysetInfo.Picking, 2500);
		KeybdEventContinuedWithSubKey(VK_RIGHT, KeysetInfo.Picking, 6000);
	}
};

class KaungRaid : protected BossRaid
{
protected:
	virtual void Transplanter(const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		string StrSkills;
		switch (CharacterInfo.Code)
		{
		case USERCONF::CODE_PROPRE::닼나:
			StrSkills = "1234!@#EQ"; break;
		case USERCONF::CODE_PROPRE::썬콜:
			Reserved1 = 2000;
			StrSkills = "P12345!@TRFZDWQ"; break;
		case USERCONF::CODE_PROPRE::신궁:
			StrSkills = "O123!@#Q"; break;
		case USERCONF::CODE_PROPRE::나로:
			StrSkills = "P12!@#$TRQ"; break;
		case USERCONF::CODE_PROPRE::듀블:
			StrSkills = "12!@F#TEQ"; break;
		case USERCONF::CODE_PROPRE::소마:
			StrSkills = "1234!#TRE"; break;
		case USERCONF::CODE_PROPRE::윈브:
			StrSkills = "O12@TREAQ"; break;
		case USERCONF::CODE_PROPRE::나워:
			StrSkills = "P`1235!@#TEWRFQ"; break;
		case USERCONF::CODE_PROPRE::스커:
			StrSkills = "ZQ"; break;
		case USERCONF::CODE_PROPRE::아란:
			Reserved1 = 1000;
			StrSkills = "23!LT0RZW"; break;
		case USERCONF::CODE_PROPRE::메세:
			StrSkills = "1!@#TRQ"; break;
		case USERCONF::CODE_PROPRE::팬텀:
			StrSkills = "O123FTWDRA"; break;
		case USERCONF::CODE_PROPRE::루미:
			StrSkills = "S"; break;
		case USERCONF::CODE_PROPRE::은월:
			StrSkills = "1@$TEW"; break;
		case USERCONF::CODE_PROPRE::데슬:
			StrSkills = "1P2L!@T$RW"; break;
		case USERCONF::CODE_PROPRE::데벤:
		case USERCONF::CODE_PROPRE::데벤2:
			Reserved1 = 1;
			StrSkills = "1234!@TE#Q"; break;
		case USERCONF::CODE_PROPRE::배메:
			StrSkills = "LP12!DFQ@#"; break;
		case USERCONF::CODE_PROPRE::메카닉:
			StrSkills = "@#$REWQ"; break;
		case USERCONF::CODE_PROPRE::제로:
			StrSkills = "TRQ"; break;
		case USERCONF::CODE_PROPRE::키네:
			StrSkills = "1Z@TPERFQ"; break;
		case USERCONF::CODE_PROPRE::아크:
			StrSkills = "O1!#TRQ"; break;
		case USERCONF::CODE_PROPRE::아델:
			StrSkills = "56TRAESQ"; break;
		case USERCONF::CODE_PROPRE::호영:
			StrSkills = "1P@TRFQ"; break;
		}
		TransplanterStringÀVecSkills(CharacterInfo.Code, StrSkills);
	}
public:
	void Préparer(
		const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		KeybdEvent(KeysetInfo.Potion);
		switch (CharacterInfo.Code)
		{
		default:
			break;
		case USERCONF::CODE_PROPRE::메카닉:
		case USERCONF::CODE_PROPRE::엔버:
			KeybdEvent('W', 1000);
		}

		AttendreJusqueDisponible(VecSkills);
		UseSkills(SKILL::BUF | SKILL::ONOFF);
		Sleep(200);
	}
	void Play(
		const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		seconds LimiteDeTemps;
		if (USERCONF::CODE_PROPRE::데벤 == CharacterInfo.Code ||
			USERCONF::CODE_PROPRE::데벤2 == CharacterInfo.Code)
		{
			LimiteDeTemps = seconds(180);
		}
		else
		{
			LimiteDeTemps = seconds(90);
		}

		// Batailler
		CombattreBoss(
			[&CharacterInfo]() {
				Sleep(100);
				MouseEvent({ 800, 320 }, CLIC_GAUCHE, 500);

				switch (CharacterInfo.Code)
				{
				default:
					break;

				case USERCONF::CODE_PROPRE::아크:
					KeybdEvent('Z', 800);
					break;
				}
			},
			LimiteDeTemps,
			SKILL::UNITARY | SKILL::RAPID | SKILL::ASSIST | SKILL::INSTALLATION | SKILL::ÉPHÉMÈRE);

		// Finir
		Sleep(1000);
		switch (CharacterInfo.Code)
		{
		default:
			break;

		case USERCONF::CODE_PROPRE::제로:
			KeybdEvent('X', 1200);
			break;

		case USERCONF::CODE_PROPRE::아크:
			KeybdEvent({ 'X', 'Z' }, 1400);
			break;
		}
		KeybdEventContinuedWithSubKey(VK_LEFT, KeysetInfo.Picking, 2000);
		KeybdEventContinuedWithSubKey(VK_RIGHT, KeysetInfo.Picking, 5500);
	}
};

class PapulatusRaid : public BossRaid
{
protected:
	virtual void Transplanter(const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		string StrSkills;
		switch (CharacterInfo.Code)
		{
		case USERCONF::CODE_PROPRE::닼나:
			StrSkills = "1234EQ"; break;
		case USERCONF::CODE_PROPRE::썬콜:
			StrSkills = "FRZDWQ"; break;
		case USERCONF::CODE_PROPRE::신궁:
			StrSkills = "O123Q"; break;
		case USERCONF::CODE_PROPRE::나로:
			StrSkills = "12RQ"; break;
		case USERCONF::CODE_PROPRE::듀블:
			StrSkills = "2ERQ"; break;
		case USERCONF::CODE_PROPRE::소마:
			StrSkills = "124@Q"; break;
		case USERCONF::CODE_PROPRE::윈브:
			StrSkills = "O12REAQ"; break;
		case USERCONF::CODE_PROPRE::나워:
			StrSkills = "P1EWFRQ"; break;
		case USERCONF::CODE_PROPRE::스커:
			StrSkills = "!@ZQ"; break;
		case USERCONF::CODE_PROPRE::아란:
			StrSkills = "RW"; break;
		case USERCONF::CODE_PROPRE::메세:
			StrSkills = "1@#RQ"; break;
		case USERCONF::CODE_PROPRE::루미:
			StrSkills = "LWRQ"; break;
		case USERCONF::CODE_PROPRE::팬텀:
			StrSkills = "12SRDWA"; break;
		case USERCONF::CODE_PROPRE::데슬:
			StrSkills = "L@W"; break;
		case USERCONF::CODE_PROPRE::데벤:
		case USERCONF::CODE_PROPRE::데벤2:
			StrSkills = "123EQ"; break;
		case USERCONF::CODE_PROPRE::배메:
			StrSkills = "L12#DQ"; break;
		case USERCONF::CODE_PROPRE::메카닉:
			StrSkills = "$REQ"; break;
		case USERCONF::CODE_PROPRE::제로:
			StrSkills = "#Q"; break;
		case USERCONF::CODE_PROPRE::키네:
			StrSkills = "!PEMQ"; break;
		case USERCONF::CODE_PROPRE::아크:
			StrSkills = "EQ"; break;
		case USERCONF::CODE_PROPRE::아델:
			StrSkills = "ESQ"; break;
		case USERCONF::CODE_PROPRE::호영:
			StrSkills = "1PFQ"; break;
		}
		TransplanterStringÀVecSkills(CharacterInfo.Code, StrSkills);
	}
public:
	void Play(
		const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		// Préparer à batailler
		MinimapRecognizer Recognizer({ 10, 80, 260, 150 });
		Recognizer.BougerÀCritèreRelatif(91);

		Sleep(800);
		ClientApi::Jump(CharacterInfo.Code);
		KeybdEvent(KeysetInfo.Potion);

		AttendreJusqueDisponible(VecSkills);
		if (TRUE != ClientApi::JeteItem(Read(TARGET_DIR "item//fragment.jpg")))
		{
			throw "파편 드롭 실패";
		}

		UseSkills(SKILL::BUF | SKILL::ONOFF);
		Sleep(1200);
		switch (CharacterInfo.Code)
		{
		case USERCONF::CODE_PROPRE::신궁:
		case USERCONF::CODE_PROPRE::나로:
		case USERCONF::CODE_PROPRE::메세:
		case USERCONF::CODE_PROPRE::키네:
			KeybdEventContinued(VK_RIGHT, GET_DURATION(50000, CharacterInfo.Vitesse));
			KeybdEventContinued(VK_LEFT, 200);
			break;
		default:
			KeybdEventContinued(VK_LEFT, GET_DURATION(12000, CharacterInfo.Vitesse));
		}

		Sleep(400);
		ClientApi::DownJump();
		Sleep(3600);

		// Appeler le boss
		switch (CharacterInfo.Code)
		{
		case USERCONF::CODE_PROPRE::닼나:
		case USERCONF::CODE_PROPRE::신궁:
		case USERCONF::CODE_PROPRE::썬콜:
			KeybdEvent('A', 1000);
			break;
		case USERCONF::CODE_PROPRE::소마:
		case USERCONF::CODE_PROPRE::배메:
			KeybdEvent('E', 1000);
			break;
		case USERCONF::CODE_PROPRE::데슬:
			KeybdEvent('D', 1000);
			break;
		case USERCONF::CODE_PROPRE::메카닉:
		case USERCONF::CODE_PROPRE::스커:
		case USERCONF::CODE_PROPRE::아란:
			KeybdEvent('W', 1000);
			break;
		case USERCONF::CODE_PROPRE::팬텀:
			KeybdEvent('Z', 1000);
			break;
		case USERCONF::CODE_PROPRE::루미:
			KeybdEvent('S', 1000);
			break;
		case USERCONF::CODE_PROPRE::듀블:
		case USERCONF::CODE_PROPRE::나로:
			KeybdEvent('Z', 1400);
			break;
		case USERCONF::CODE_PROPRE::데벤:
		case USERCONF::CODE_PROPRE::데벤2:
		case USERCONF::CODE_PROPRE::아델:
			KeybdEvent('Q', 1000);
			break;
		case USERCONF::CODE_PROPRE::아크:
		case USERCONF::CODE_PROPRE::제로:
		case USERCONF::CODE_PROPRE::메세:
			KeybdEvent('F', 1000);
			break;
		}
		Sleep(600);

		// Batailler
		CombattreBoss(
			[&CharacterInfo]() {
				Sleep(500);
				MouseEvent({ 800, 320 }, CLIC_GAUCHE, 800);

				switch (CharacterInfo.Code)
				{
				default:
					break;

				case USERCONF::CODE_PROPRE::아크:
					KeybdEvent('Z', 800);
					break;
				}

				KeybdEventContinued(VK_RIGHT, GET_DURATION(400000, CharacterInfo.Vitesse));
			},
			seconds(120),
				SKILL::UNITARY | SKILL::RAPID | SKILL::ASSIST | SKILL::INSTALLATION | SKILL::ÉPHÉMÈRE);

		// Finir
		Sleep(1000);
		switch (CharacterInfo.Code)
		{
		default:
			break;

		case USERCONF::CODE_PROPRE::제로:
			KeybdEvent('X', 1200);
			break;

		case USERCONF::CODE_PROPRE::아크:
			KeybdEvent({ 'X', 'Z' }, 1400);
			break;
		}
		KeybdEventContinuedWithSubKey(VK_LEFT, KeysetInfo.Picking, 4000);
		KeybdEventContinuedWithSubKey(VK_RIGHT, KeysetInfo.Picking, 6000);
	}
};

class Boss :
	public ZacumRaid, public RootAbyssBloodyQueenRaid, public RootAbyssVonBanRaid, public BellumRaid,
	public VonLeonRaid, public KaungRaid, public PapulatusRaid
{
public:
	typedef string BOSSRAID_EXCEPTION_DESCRIPTION;
	template<class RAID_T>
	void Joeur(void (RAID_T::* Play)(const USERCONF::CHARACTER_INFO&), string NomDeBoss, const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		auto FaireDescription = [&NomDeBoss](string Descrption)
		{
			return NomDeBoss + " 레이딩 실패 (" + Descrption + ")";
		};

		try
		{
			RAID_T::Play(CharacterInfo);
		}
		catch (const MalrangiException const& e)
		{
			throw FaireDescription(e.what());
		}
		catch (const char* des)
		{
			throw FaireDescription(des);
		}
		catch (...)
		{
			throw FaireDescription("MalrangiException 객체 또는 문자열 형식이 아닌 예외가 throw됨.");
		}
	};
};