#pragma once

#if (defined(_MSC_VER) && (_MSC_VER >= 1800))
#include <ppl.h>
#include <ppltasks.h>
namespace pplx = concurrency;
#else
#include <pplx/pplx.h>
#include <pplx/pplxtasks.h>
#include <unistd.h>
namespace pplx {
	inline static void wait (int milliseconds) {
        pplx::create_task([milliseconds]{
        	usleep(milliseconds*1000);
        }).wait();
    }
}
#endif
