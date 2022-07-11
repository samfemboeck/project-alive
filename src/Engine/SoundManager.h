class SoundManager
{
public:
	static void init();
	static void add(std::string filename);
	static void oneShot(std::string filename);
	static void start(std::string filename);
	static void stop(std::string filename);
	static bool isPlaying(std::string filename);
	static bool isEnd(std::string filename);
	static void reset(std::string filename);
	static void fade(std::string filename, float begin, float end, float millis);
};