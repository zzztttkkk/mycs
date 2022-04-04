//
// Created by ztk on 2022/4/2.
//

#pragma once

#include <asio.hpp>

namespace mycs::http {

class StreamBuf {
   private:
	asio::streambuf raw;
   public:
	void write() {}
};

}  // namespace mycs::http