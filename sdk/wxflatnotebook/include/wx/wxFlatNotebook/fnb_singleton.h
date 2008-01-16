#ifndef FNB_SINGLETON_H
#define FNB_SINGLETON_H

/**
 * A template class that implements the wxFNBSingleton pattern.
 *
 * \date 08-23-2006
 * \author eran
 */
template <typename T>
class wxFNBSingleton
{
	static T* ms_instance;
public:
	/**
	 * Static method to access the only pointer of this instance.
	 * \return a pointer to the only instance of this 
	 */
	static T* Get();

	/**
	 * Release resources.
	 */
	static void Free();

protected:
	/**
	 * Default constructor.
	 */
	wxFNBSingleton();

	/**
	 * Destructor.
	 */
	virtual ~wxFNBSingleton();
};
template <typename T>
T* wxFNBSingleton<T>::ms_instance = 0;

template <typename T>
wxFNBSingleton<T>::wxFNBSingleton()
{
}

template <typename T>
wxFNBSingleton<T>::~wxFNBSingleton()
{
}

template <typename T>
T* wxFNBSingleton<T>::Get()
{
	if(!ms_instance)
		ms_instance = new T();
	return ms_instance;
}

template <typename T>
void wxFNBSingleton<T>::Free()
{
	if( ms_instance )
	{
		delete ms_instance;
		ms_instance = 0;
	}
}

#endif // FNB_SINGLETON_H
