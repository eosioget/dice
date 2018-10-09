#include "sicbo.hpp"
#include "library.hpp"

// 创建游戏
void sicbo::creategame(const uint64_t id, const checksum256& commitment) {    
    eosio_assert(id >= 201809140001, "id is too small");
    eosio_assert(id <= 301809140001, "id is too big");

    // 需要合约账号的权限
    require_auth( _self );

    // 判断最后一个游戏 是否 在进行中
    if (games.begin() != games.end()) {
        auto game_ritr = games.rbegin();
        logger_info("game_ritr->id:", game_ritr->id);
        eosio_assert(game_ritr->game_state != 0, "some game is going");
        eosio_assert(game_ritr->id < id, "has game_id bigger than id");
    }

    // 创建一个新游戏对象
    games.emplace(_self, [&](auto& new_game){
        // 这里要注意类型，要有一个数字是uint_64的类型才行
        new_game.id          = id;
        new_game.create_time = eosio::time_point_sec(now());
        new_game.update_time = eosio::time_point_sec(now());
        new_game.ttl = 80;
        new_game.commitment  = commitment;
        logger_info("new_game.id:", new_game.id);
    });
}

// 开奖接口
void sicbo::reveal(const uint64_t gameid, const checksum256& source) {
    eosio_assert(gameid > 0, "gameid is not valid");
    logger_info("gameid:", gameid)
    // 需要合约账号的权限
    require_auth( _self );
    // 判断gameid是否存在
    auto game = games.get(gameid);
    eosio_assert(gameid == game.id, "gameid is not valid");
    // 判断游戏是否在进行中
    eosio_assert(0 == game.game_state, "game_state is not valid");

    logger_info("assert_sha256 start")
    // 判断签名是否一致
    char *c_src = (char *) &source;
    assert_sha256(c_src, sizeof(source), &game.commitment);
    logger_info("assert_sha256 success")

    // ++++++++++  开奖逻辑 ++++++++++++++++
    // 生成随机数
    int num1 = (int)(source.hash[0] + source.hash[1] + source.hash[2] + source.hash[3] + source.hash[4] + source.hash[5]) % 6 + 1;
    int num2 = (int)(source.hash[6] + source.hash[7] + source.hash[8] + source.hash[9] + source.hash[10] + source.hash[11]) % 6 + 1;
    int num3 = (int)(source.hash[12] + source.hash[13] + source.hash[14] + source.hash[15] + source.hash[16] + source.hash[17]) % 6 + 1;
    logger_info("1th: ", num1, ", 2th: ", num2, ", 3th: ", num3)

    // 修改游戏表
    games.modify(game, 0, [&](auto& game2) {
        game2.num1 = num1;
        game2.num2 = num2;
        game2.num3 = num3;
		game2.source = source;
        game2.game_state = 1;
        game2.update_time = eosio::time_point_sec(now());
    });

    // 计算中奖结果
    MAP_RESULT result = this->_getWinsMap(num1, num2, num3);

    // 修改下注表
    auto idx = offers.template get_index<N(gameid)>();
    logger_info("find gameid:", gameid)
    auto offer_iter = idx.find(gameid);

    // 遍历所有的数据，修改赌注结果
    int i = 0;
    while (offer_iter != idx.end()) {
        if (offer_iter->result != 0) {
            offer_iter++;
            continue;
        } 

        // 修改赌注结果
        offers.modify(*offer_iter, 0, [&](auto& offer){
            if (result[offer_iter->offer_type]) {
                // 胜利
                offer.result = 1;
            } else {
                // 失败
                offer.result = 2;
            }
            offer.update_time = eosio::time_point_sec(now());
        });
        offer_iter++;
    }

    // 设置开奖完毕
    games.modify(game, 0, [&](auto& game2) {
        game2.game_state = 2;
        game2.update_time = eosio::time_point_sec(now());
    });
}

// 通过结果计算所有下注类型是否胜利
MAP_RESULT sicbo::_getWinsMap(const uint8_t num1, const uint8_t num2, const uint8_t num3) {
    MAP_RESULT map;
    int sum = num1 + num2 + num3;

    if (num1 == num2 && num1 == num3) {
        // 判断围骰的情况
        map[num1 * 100 + num2 * 10 + num3] = true;
        // 只要是三个数就算
        map[three_n] = true;
    } else {
        // 判断大小
        if (sum >= 4 && sum <= 10) {
            map[small] = true;
        } else if (sum >= 11 && sum <= 17) {
            map[big] = true;
        }

        // 判断单双
        if (sum % 2) {
            map[odd] = true;
        } else {
            map[even] = true;
        }
    }

    // 判断点数总和
    if (sum >= 4 && sum <= 17) {
        map[sum] = true;
    }

    // 设置单骰
    map[num1 * 100] = true;
    map[num2 * 100] = true;
    map[num3 * 100] = true;

    // 设置双骰子
    int double_num = 0;
    if (num1 == num2) {
        double_num = num1;
    } else if (num2 == num3) {
        double_num = num2;
    } else if (num1 == num3) {
        double_num = num1;
    } 

    if (double_num > 0) {
        map[double_num * 100 + double_num * 10] = true;
    }

    return map;
}


asset sicbo::str2eos(const std::string eosnum)
{
	asset result;

	result.amount = atoi(eosnum.c_str());
	result.symbol = S(4, EOS);

	return result;
}

void sicbo::require_offertype(int sicboplay)
{
	switch(sicboplay)
	{
		case small:
		case big:
		case odd:
		case even:
		case sum_4:
		case sum_5:
		case sum_6:
		case sum_7:
		case sum_8:
		case sum_9:
		case sum_10:
		case sum_11:
		case sum_12:
		case sum_13:
		case sum_14:
		case sum_15:
		case sum_16:
		case sum_17:
		case single_1:
		case single_2:
		case single_3:
		case single_4:
		case single_5:
		case single_6:
		case double_1:
		case double_2:
		case double_3:
		case double_4:
		case double_5:
		case double_6:
		case three_1:
		case three_2:
		case three_3:
		case three_4:
		case three_5:
		case three_6:
		case three_n:
			break;
		default:
			eosio_assert(0, "offertype error");
	}

	return;
}

BET_RESULT sicbo::_split_extra(const std::string strlist, uint64_t* gameidptr)
{
	std::string data = strlist;

	eosio_assert(data.length(), "offerbet is null");

	#define SPLITONE(x) \
		(x).substr(0, (x).find("|")); \
		(x) = (x).erase(0, (x).find("|")+1);

	if (gameidptr)
	{
		*gameidptr = 0;

		if (data.find("|") != std::string::npos)
		{
			auto gameidstr = SPLITONE(data);
			eosio_assert(isdecnum(gameidstr), "gameid isn't decimal num");

			*gameidptr = atoll(gameidstr.c_str());
		}
	}

	BET_RESULT map;

	while(data.find("|") != std::string::npos)
	{
		auto eos  = SPLITONE(data);
		auto play = SPLITONE(data);

		eosio_assert(play.length(), "betlist format error");
		eosio_assert(isdecnum(eos), "eos isn't decimal num");
		eosio_assert(isdecnum(play), "play isn't decimal num");

		int key = atoi(play.c_str());

		require_offertype(key);

		if (map.find(key) != map.end()) {
			map[key] += str2eos(eos);
		} else {
			map[key]  = str2eos(eos);
		}
	}
	
	#undef SPLITONE

	return map;
}

