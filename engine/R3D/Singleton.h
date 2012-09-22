#ifndef  _SINGLETON_H
#define  _SINGLETON_H
namespace R3
{
    template<typename T>
	class Singleton
    {
        public:
			static T* Instance()
			{
				if (instance == 0)
					instance=new T();
				return instance;
			}
			static void Destroy()
			{
				instance=0;
				destroyed=true;
			}

        protected:
            Singleton() {};
			~Singleton() {};
            static T *instance;
			bool destroyed;
    };
	template<typename T> T* Singleton<T>::instance = 0; 
}
#endif