//
// Created by ztk on 2022/4/1.
//

#pragma once

namespace mycs::simplehttp {

struct ProtocolOption {
   public:
	int MaxMessageLineSize = 0;
	int MaxMessageBodySize = 0;
	int MaxAliveSeconds = 0;
};

}  // namespace mycs::simplehttp
