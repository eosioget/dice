#include <vector>
#include <string>
#include <eosiolib/eosio.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/contract.hpp>
#include <eosiolib/crypto.h>
#include <map>
#include <cstdlib>

using eosio::key256;
using eosio::indexed_by;
using eosio::const_mem_fun;
using eosio::asset;
using eosio::permission_level;
using eosio::action;
using eosio::print;
using eosio::name;

#define DEBUG
typedef std::map<int, bool> MAP_RESULT;
typedef std::map<int, asset> BET_RESULT;

// 下注类型
enum enum_offer_type { 
    small = 0,
    big = 1, 
    odd = 3,
    even = 2,

    sum_4 = 4,
    sum_5 = 5,
    sum_6 = 6,
    sum_7 = 7,
    sum_8 = 8,
    sum_9 = 9,
    sum_10 = 10,
    sum_11 = 11,
    sum_12 = 12,
    sum_13 = 13,
    sum_14 = 14,
    sum_15 = 15,
    sum_16 = 16,
    sum_17 = 17,

    single_1 = 100,
    single_2 = 200,
    single_3 = 300,
    single_4 = 400,
    single_5 = 500,
    single_6 = 600,

    double_1 = 110,
    double_2 = 220,
    double_3 = 330,
    double_4 = 440,
    double_5 = 550,
    double_6 = 660,

    three_1 = 111,
    three_2 = 222,
    three_3 = 333,
    three_4 = 444,
    three_5 = 555,
    three_6 = 666,
    three_n = 777, // 只要是三个数就算
};


class sicbo : public eosio::contract {
  public:
    sicbo(account_name self):eosio::contract(self),
       offers(_self, _self),
       games(_self, _self)
    {}

    //@abi action
    void creategame(const uint64_t id, const checksum256& commitment);
    //@abi action
    void offerbet(const asset bet, const account_name player, const uint64_t gameid, const std::string& betlist);
    //@abi action
    void reveal(const uint64_t gameid, const checksum256& source);
    //@abi action
    void balance(const uint64_t gameid, const account_name player);
    //@abi action
    void cleargame(const uint64_t gameid);
    //@abi action
    void clearoffer(const uint64_t gameid);
    //@abi action
    void closegame(const uint64_t gameid);
    void transfer(account_name from, account_name to, const asset& quantity, const std::string& memo);//注意没有action

  private:
    // 赔率类型
    std::map<int,int> wins_map = { 
        { small, 2 },
        { big, 2 },
        { odd, 2 },
        { even, 2 },

        { sum_4, 60 },
        { sum_5, 30 },
        { sum_6, 17 },
        { sum_7, 12 },
        { sum_8, 8 },
        { sum_9, 6 },
        { sum_10, 6 },
        { sum_11, 6 },
        { sum_12, 6 },
        { sum_13, 8 },
        { sum_14, 12 },
        { sum_15, 17 },
        { sum_16, 30 },
        { sum_17, 60 },

        { single_1, 2 },
        { single_2, 2 },
        { single_3, 2 },
        { single_4, 2 },
        { single_5, 2 },
        { single_6, 2 },

        { double_1, 10 },
        { double_2, 10 },
        { double_3, 10 },
        { double_4, 10 },
        { double_5, 10 },
        { double_6, 10 },

        { three_1, 185 },
        { three_2, 185 },
        { three_3, 185 },
        { three_4, 185 },
        { three_5, 185 },
        { three_6, 185 },
        { three_n, 30 }
    };
      
    //@abi table game i64 赌局表
    struct game {
        uint64_t id;  // 赌局id，如:201809130001
        uint32_t ttl = 120; // 倒计时时间，默认2分钟
        uint8_t game_state = 0; // 状态：0:进行中, 1:开奖中, 2:已结束
		checksum256 source;// 开奖种子
        checksum256 commitment; // 随机数加密串
        uint8_t num1 = 0; // 第1个骰子数字
        uint8_t num2 = 0; // 第2个骰子数字
        uint8_t num3 = 0; // 第3个骰子数字
        eosio::time_point_sec create_time; // 创建时间
        eosio::time_point_sec update_time; // 更新时间
        uint64_t primary_key() const { return id; }
        EOSLIB_SERIALIZE( game, (id)(ttl)(game_state)(source)(commitment)(num1)(num2)(num3)(create_time)(update_time) )
    };
    
    //@abi table offer i64 赌注明细表
    struct offer {
        uint64_t          id; // 下注id
        account_name      player; // 用户名
        asset             bet; // 赌注
        uint64_t          gameid; // 赌局id，如:201809130001
        uint32_t          offer_type; // 下注类型：1:大, 2:小
        uint8_t           bet_odds = 2; // 下注赔率，默认：x2
        uint8_t result = 0; // 赌注结果：0:未开奖, 1:胜利, 2:失败
        uint8_t tranffer_state = 0; // 转账状态：0:未转账, 1:转账中, 2:转账成功, 3:转账失败
        eosio::time_point_sec create_time; // 创建时间
        eosio::time_point_sec update_time; // 更新时间
        
        uint64_t primary_key() const { return id; }
        uint64_t by_gameid() const { return gameid; } // 可以通过赌局id查询数据
        account_name by_account_name() const { return player; } // 可以通过用户名查询数据
        EOSLIB_SERIALIZE( offer, (id)(player)(bet)(gameid)(offer_type)(bet_odds)(result)(tranffer_state)(create_time)(update_time) )
    };

    // 创建一个多索引容器的游戏列表
    typedef eosio::multi_index< N(game), game> game_index;

    // // 创建一个多索引容器的赌注列表
    typedef eosio::multi_index< N(offer), offer,
        indexed_by< N(gameid), const_mem_fun<offer, uint64_t, &offer::by_gameid > >,
        indexed_by< N(player), const_mem_fun<offer, account_name,  &offer::by_account_name> >
    > offer_index;
    // typedef eosio::multi_index< N(offer), offer> offer_index;


    game_index games;
    offer_index offers;

    // 通过结果计算所有下注类型是否胜利
    MAP_RESULT _getWinsMap(const uint8_t num1, const uint8_t num2, const uint8_t num3);
    // bool _changeState(auto offer_iter, const int tranffer_state);

	// 分割与验证多注
	BET_RESULT _split_extra(const std::string data, uint64_t* gameidptr=nullptr);	//0: offerbet, 1: transfer
	asset str2eos(const std::string eosnum);
	void require_offertype(int sicboplay);
};

//注意没有offerbet
#define EOSIO_ABI_EX( TYPE, MEMBERS ) \
 extern "C" { \
    void apply(uint64_t receiver, uint64_t code, uint64_t action) { \
       if (action == N(onerror)) { \
          eosio_assert(code == N(eosio), "onerror action's are only valid from the \"eosio\" system account"); \
       } \
       auto self = receiver; \
       if ((code==N(eosio.token) && action==N(transfer)) || (code==self && (action==N(creategame) || action==N(reveal) || action==N(balance) || action==N(cleargame) || action==N(clearoffer) || action==N(closegame))) ) { \
          TYPE thiscontract( self ); \
          switch( action ) { \
             EOSIO_API( TYPE, MEMBERS ) \
          } \
       } \
    } \
 }

EOSIO_ABI_EX(sicbo, (creategame)(reveal)(balance)(cleargame)(clearoffer)(closegame)(transfer))
