//
// Created by ztk on 2022/4/1.
//

#pragma once

namespace mycs::simplehttp {

struct ProtocolOption {
   public:
	int MaxUrlSize = 0;
	int MaxHeaderLineSize = 0;
	int MaxRequestBodySize = 0;
	int MaxResponseBodySize = 0;
	int MaxAliveSeconds = 0;
};

}  // namespace mycs::simplehttp
