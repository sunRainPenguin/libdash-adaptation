#ifndef LIBDASH_FRAMEWORK_INPUT_IMEDIAOBJECTOBSERVER_H_
#define LIBDASH_FRAMEWORK_INPUT_IMEDIAOBJECTOBSERVER_H_

namespace libdash
{
	namespace framework
	{
		namespace input
		{
			class IMediaObjectObserver
			{
			public:
				virtual ~IMediaObjectObserver () {}
				virtual	void OnDownloadRateChanged    (uint64_t bytesDownloaded) = 0;
			};
		}
	}
}
#endif /* LIBDASH_FRAMEWORK_INPUT_IMEDIAOBJECTOBSERVER_H_ */
