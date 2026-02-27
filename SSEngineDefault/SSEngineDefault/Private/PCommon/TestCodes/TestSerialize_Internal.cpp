#include "SSEngineDefault/Public/CommonSerializer/DefaultTypeSerializsers.h"



void StringSerializerTest()
{
	SS::PooledList<SS::StringW> OriginalStrings(20);

	{
		SS::StringW EmptyStr;

		SS::StringW TestStr1 = L"너는 나를 존중해야 한다. 나는 발롱도르 5개와 수많은 개인 트로피를 들어올렸으며 "
			L"2016 유로에서 포르투갈을 이끌고 우승을 차지했고 동시에 A매치 역대 최다 득점자이다. "
			L"또한 챔스 역대 최다 득점자이자 5번이나 우승을 차지한 레알 마드리드의 상징이다. "
			L"또한 36세의 나이에도 프리미어 리그에서 18골을 기록하고 챔스에서 5경기 연속 골을 기록하며"
			L" 내가 세계 최고임을 증명해냈다. 은혜를 모르는 맨유 보드진과 팬들은 내가 맨유의 골칫덩이라고"
			L" 쫓아냈지만, 내가 세계 최고이고 내가 팀보다 위대하다는 사실은 바뀌지 않는다. 내가 사우디에 간"
			L" 이유는 메시에 대한 자격지심이 아니라 유럽에서 이룰 수 있는 모든 것을 이루었기에 아시아를 정복하기"
			L" 위해 간 것이지, 단지 돈을 위해서 간 것이 아니다.";

		SS::StringW TestStr2 = "HelloHelloab";
		SS::StringW TestStr3 = "abcdefgHelloijklmnop";

		SS::StringW TestStr4 = L"너는 호날두를 존중해야 한다. 호날두는 발롱도르 5개와 수많은 개인 트로피를 들어올렸으며"
			L" 2016 유로에서 포르투갈을 이끌고 우승을 차지했고 동시에 A매치 역대 최다 득점자이다. "
			L"또한 챔스 역대 최다 득점자이자 5번이호날두 우승을 차지한 레알 마드리드의 상징이다. "
			L"또한 36세의 호날두이에도 프리미어 리그에서 18골을 기록하고 챔스에서 5경기 연속 골을 기록하며 "
			L"내가 세계 최고임을 증명해냈다. 은혜를 모르는 맨유 보드진과 팬들은 내가 맨유의 골칫덩이라고 "
			L"쫓아냈지만, 내가 세계 최고이고 내가 팀보다 위대하다는 사실은 바뀌지 않는다. 내가 사우디에 간 "
			L"이유는 메시에 대한 자격지심이 아니라 유럽에서 이룰 수 있는 모든 것을 이루었기에 아시아를 정복하기"
			L" 위해 간 것이지, 단지 돈을 위해서 간 것이 아니다.";

		SS::StringW TestStr5 = "Hello Hello Hello Hello Hello";

		SS::StringW TestStr6 = L"너는 나를 존중해야 한다. 나는 발롱도르 5개와 수많은 개인 트로피를 들어올렸으며 "
			L"2016 유로에서 포르투갈을 이끌고라니 우승을 차지했고라니 동시에 A매치 역대 최다 득점자이다. "
			L"또한 챔스 역대 최다 득점자이자 5번이나 우승을 차지한 레알 마드리드의 상징이다. "
			L"또한 36세의 나이에도 프리미어 리그에서 18골을 기록하고라니 챔스에서 5경기 연속 골을 기록하며"
			L" 내가 세계 최고라니임을 증명해냈다. 은혜를 모르는 맨유 보드진과 팬들은 내가 맨유의 골칫덩이라고"
			L"라니 쫓아냈지만, 내가 세계 최고라니이고라니 내가 팀보다 위대하다는 사실은 바뀌지 않는다. 내가 사우디에 간"
			L" 이유는 메시에 대한 자격지심이 아니라 유럽에서 이룰 수 있는 모든 것을 이루었기에 아시아를 정복하기"
			L" 위해 간 것이지, 단지 돈을 위해서 간 것이 아니다.";

		SS::StringW TestStr7 = L"안녕하시와요";


		OriginalStrings.PushBack(EmptyStr);
		OriginalStrings.PushBack(TestStr1);
		OriginalStrings.PushBack(TestStr2);
		OriginalStrings.PushBack(TestStr3);
		OriginalStrings.PushBack(TestStr4);
		OriginalStrings.PushBack(TestStr5);
		OriginalStrings.PushBack(TestStr6);
		OriginalStrings.PushBack(TestStr7);
	}

	SS::PooledList<byte> Data;
	SS::PooledList<SS::StringW> Strings = OriginalStrings;


	Data.Clear();
	int WriteBytes = AppendDataFromStrings(Data, Strings);
	if (_CrtCheckMemory() == false) SS_INTERRUPT();

	int ReadBytes = FillStringFromData(Strings, Data);
	if (_CrtCheckMemory() == false) SS_INTERRUPT();

	SS_ASSERT(WriteBytes > 0 && ReadBytes > 0 && WriteBytes == ReadBytes);
	for (int32 i = 0; i < OriginalStrings.GetSize(); i++)
	{
		SS_ASSERT(OriginalStrings[i] == Strings[i]);
	}

	SS::StringW LemonPhrase =
		L"夢ならばどれほどよかったでしょう"
		L" 未だにあなたのことを夢にみる "
		L"忘れた物を取りに帰るように "
		L"古びた思い出の埃を払う "
		L"戻らない幸せがあることを "
		L"最後にあなたが教えてくれた"
		L"言えずに隠してた昏い過去も"
		L"あなたがいなきゃ永遠に昏いまま"
		L"きっともうこれ以上傷つくことなど"
		L"ありはしないとわかっている"
		L"あの日の悲しみさえ あの日の苦しみさえ"
		L"そのすべてを愛してた あなたとともに"
		L"胸に残り離れない 苦いレモンの匂い"
		L"雨が降り止むまでは帰れない"
		L"今でもあなたはわたしの光"
		L"暗闇であなたの背をなぞった"
		L"その輪郭を鮮明に覚えている"
		L"受け止めきれないものと出会うたび"
		L"溢れてやまないのは涙だけ"
		L"何をしていたの 何を見ていたの"
		L"わたしの知らない横顔で"
		L"どこかであなたが今"
		L"わたしと同じ様な"
		L"涙にくれ淋しさの中にいるなら"
		L"わたしのことなどどうか忘れてください"
		L"そんなことを心から願うほどに"
		L"今でもあなたはわたしの光"
		L"自分が思うより 恋をしていたあなたに"
		L"あれから思うように 息ができない"
		L"あんなに側にいたのに まるで嘘みたい"
		L"とても忘れられない それだけが確か"
		L"あの日の悲しみさえ あの日の苦しみさえ"
		L"そのすべてを愛してた あなたとともに"
		L"胸に残り離れない苦いレモンの匂い";

	SS::StringW BanDPhrase =
		L"지금당장떠나면아무도다치지않는다그러지않으면너희는모두죽어탐정놀이도이젠끝이다현실로돌아"
		L"가면잊지말고전해라스텔라론헌터가너희의마지막을배웅했다는것을소탕시작액션원집행목표고정즉"
		L"시처단프로토콜통과초토화작전집행지금당장떠나면아무도다치지않는다그러지않으면너희는모두죽"
		L"어탐정놀이도이젠끝이다현실로돌아가면잊지말고전해라스텔라론헌터가너희의마지막을배웅했다는"
		L"것을소탕시작액션원집행목표고정즉시처단프로토콜통과초토화작전집행지금당장떠나면아무도다치"
		L"지않는다그러지않으면너희는모두죽어탐정놀이도이젠끝이다현실로돌아가면잊지말고전해라스텔라"
		L"론헌터가너희의마지막을배웅했다는것을소탕시작액션원집행목표고정즉시처단프로토콜통과초토화"
		L"작전집행지금당장떠나면아무도다치지않는다그러지않으면너희는모두죽어탐정놀이도이젠끝이다현"
		L"실로돌아가면잊지말고전해라스텔라론헌터가너희의마지막을배웅했다는것을소탕시작액션원집행목"
		L"표고정즉시처단프로토콜통과초토화작전집행지금당장떠나면아무도다치지않는다그러지않으면너희"
		L"는모두죽어탐정놀이도이젠끝이다현실로돌아가면잊지말고전해라스텔라론헌터가너희의마지막을배"
		L"웅했다는것을소탕시작액션원집행목표고정즉시처단프로토콜통과초토화작전집행지금당장떠나면아"
		L"무도다치지않는다그러지않으면너희는모두죽어탐정놀이도이젠끝이다현실로돌아가면잊지말고전해"
		L"라스텔라론헌터가너희의마지막을배웅했다는것을소탕시작액션원집행목표고정즉시처단프로토콜통"
		L"과초토화작전집행지금당장떠나면아무도다치지않는다그러지않으면너희는모두죽어탐정놀이도이젠"
		L"끝이다현실로돌아가면잊지말고전해라스텔라론헌터가너희의마지막을배웅했다는것을소탕시작액션"
		L"원집행목표고정즉시처단프로토콜통과초토화작전집행지금당장떠나면아무도다치지않는다그러지않"
		L"으면너희는모두죽어탐정놀이도이젠끝이다현실로돌아가면잊지말고전해라스텔라론헌터가너희의마"
		L"지막을배웅했다는것을소탕시작액션원집행목표고정즉시처단프로토콜통과초토화작전집행";



	OriginalStrings.Clear();
	OriginalStrings.PushBack(L"ABCDEFGHIJKLMNOPWQRSTUVWXYZ");
	OriginalStrings.PushBack(SS::move(LemonPhrase));
	OriginalStrings.PushBack(L"ABCDEFGHIJKLMNOPWQRSTUVWXYZ");
	OriginalStrings.PushBack(SS::StringW());
	OriginalStrings.PushBack(SS::move(BanDPhrase));
	OriginalStrings.PushBack(L"ABCDEFGHIJKLMNOPWQRSTUVWXYZ");

	Strings = OriginalStrings;

	Data.Clear();
	WriteBytes = AppendDataFromStrings(Data, Strings);
	if (_CrtCheckMemory() == false) SS_INTERRUPT();

	ReadBytes = FillStringFromData(Strings, Data);
	if (_CrtCheckMemory() == false) SS_INTERRUPT();

	SS_ASSERT(WriteBytes > 0 && ReadBytes > 0 && WriteBytes == ReadBytes);
	for (int32 i = 0; i < OriginalStrings.GetSize(); i++)
	{
		SS_ASSERT(OriginalStrings[i] == Strings[i]);
	}


	int a = 0;
}


void HasherSerializerTest()
{
	SS::PooledList<SS::SHasherW> OriginalHashers(20);

	OriginalHashers.PushBack(L"Hello");
	OriginalHashers.PushBack(L"My");
	OriginalHashers.PushBack(L"Name");
	OriginalHashers.PushBack(L"Is");
	OriginalHashers.PushBack(L"Shin");
	OriginalHashers.PushBack(L"ID_01");
	OriginalHashers.PushBack(L"ID_02");
	OriginalHashers.PushBack(L"신동철");
	OriginalHashers.PushBack(L"Shin");


	SS::PooledList<byte> Data;
	SS::PooledList<SS::SHasherW> Hashers = OriginalHashers;

	Data.Clear();
	int32 WrittenBytes = AppendDataFromHashers(Data, Hashers);
	SS_ASSERT(WrittenBytes > 0);
	if (_CrtCheckMemory() == false) SS_INTERRUPT();

	int32 ReadBytes = FillHashersFromData(Hashers, Data);
	SS_ASSERT(ReadBytes > 0);
	if (_CrtCheckMemory() == false) SS_INTERRUPT();

	SS_ASSERT(WrittenBytes > 0 && ReadBytes > 0 && ReadBytes == ReadBytes);
	for (int32 i = 0; i < OriginalHashers.GetSize(); i++)
	{
		SS_ASSERT(OriginalHashers[i] == Hashers[i]);
	}

	int a = 0;
}
