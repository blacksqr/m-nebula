#ifndef N_FILETRANSFER
#define N_FILETRANSFER

#define FT_SRV_OPEN		"fopen"
#define FT_SRV_CLOSE	"fclose"
#define FT_SRV_WRITE	"fwrite"
#define	FT_SRV_READ		"fread"
#define FT_SRV_TELL		"ftell"
#define	FT_SRV_SEEK		"fseek"
#define	FT_SRV_ISOPEN	"fisopen"
#define	FT_SRV_EXISTS	"fexists"

#define FT_CLN_OPEN		"~fopen"
#define FT_CLN_CLOSE	"~fclose"
#define FT_CLN_WRITE	"~fwrite"
#define	FT_CLN_READ		"~fread"
#define FT_CLN_TELL		"~ftell"
#define	FT_CLN_SEEK		"~fseek"
#define	FT_CLN_ISOPEN	"~fisopen"
#define	FT_CLN_EXISTS	"~fexists"

#define FT_DONE			"~done"
#define FT_FAIL			"~fail"
#define FT_GO			"~go"
#define FT_EOF			"~eof"

#define FT_TRUE			"~true"
#define FT_FALSE		"~false"

#define FT_SEEK_START	"seekstart"
#define FT_SEEK_END		"seekend"
#define FT_SEEK_CURRENT	"seekcurrent"

#define  FT_PACKET_SIZE	(1024*1024*16)

#ifdef _DEBUG
#define	 FILE_TRANSFER_PROFILE
#endif

#ifdef FILE_TRANSFER_PROFILE
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#endif

#ifndef N_DEBUG_H
#include <assert.h>
#define n_assert	assert
#endif


#ifdef	FILE_TRANSFER_PROFILE
#define PROFILER_STRING 1024

class ft_profiler {
public:
	ft_profiler() : 
	  in_begin(false) 
	{};

	const char* begin(int size) 
	{
		n_assert(!this->in_begin);		
		this->start_t = get_milli();
		this->in_begin = true;
		this->packets_num = 0;
		this->min_packet_size = INT_MAX;
		this->max_packet_size = INT_MIN;
		this->total_size = size;

		sprintf(this->result, "\nFIO> transfer of size %.3fK started\n", (float)size/1024.0);

		return this->result;
	}

	void step(int received) 
	{
		n_assert(this->in_begin);
		this->packets_num++;
		if (received < this->min_packet_size) 
			this->min_packet_size = received;
		if (received > this->max_packet_size) 
			this->max_packet_size = received;
	}

	const char* end(const char* result) 
	{
		n_assert(this->in_begin);
		this->in_begin = false;
		this->finish_t = get_milli();
		long delta_t = (long)(finish_t - start_t);
		double seconds = (double)delta_t/1000.0;		
		double rate = seconds > 0.00001 ? ((double)total_size/1024.0)/seconds : -1.0;
		sprintf(this->result, 
			"FIO> transfer is %s \n\t size: %.3fK \n\t time: %.6fs \n\t transfer rate: %.3fK/s \n\t number of packets: %d \n\t min packet: %d \n\t max packet: %d \n\t average packet: %d\n",
			result, (float)total_size/1024.0, seconds, rate, packets_num, 
			min_packet_size, max_packet_size, total_size/packets_num);

		return this->result;		
	}

	int total_size;
	int packets_num;
	int min_packet_size;
	int max_packet_size;

private:
	__int64	 start_t;
	__int64	 finish_t;
	bool in_begin;

	char	result[PROFILER_STRING];

	static __int64 get_milli() {
		_timeb tm;
		_ftime(&tm);	
		return tm.time*1000 + tm.millitm;
	}
};

#endif
#endif