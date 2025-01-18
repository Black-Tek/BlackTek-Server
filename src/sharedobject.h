#ifndef BT_SHARED_OBJECT_H
#define BT_SHARED_OBJECT_H

#include <memory>

class SharedObject : public std::enable_shared_from_this<SharedObject>
{
public:
    virtual ~SharedObject() = default;

    SharedObject& operator=(const SharedObject&) = delete;
	
	template <class T, class... Args>
	static std::shared_ptr<T> Instantiate_Shared(Args... args) {
    	//std::cout << "## Debug Info : " << typeid(T).name() << " created with Instantiate_Shared()." << "\n";
    	return std::make_shared<T>(std::forward<Args>(args)...);
    }

	template <class T>
	std::shared_ptr<T> static_shared_this() {
    	//std::cout << "## Debug Info : " << typeid(T).name() << " created with static_shared_this()." << "\n";
    	//std::cout << " # " << typeid(T).name() << " has " << (shared_from_this().use_count() - 1) << " instances." << "\n";
		return std::static_pointer_cast<T>(shared_from_this());
	}

	template <class T>
	std::shared_ptr<const T> static_shared_this() const {
    	//std::cout << "## Debug Info : " << typeid(T).name() << " created with static_shared_this()." << "\n";
    	//std::cout << " # " << typeid(T).name() << " has " << (shared_from_this().use_count() - 1) << " instances." << "\n";
		return std::static_pointer_cast<const T>(shared_from_this());
	}

	template <class T>
	std::shared_ptr<T> dynamic_shared_this() {
    	//std::cout << "## Debug Info : " << typeid(T).name() << " created with dynamic_shared_this()." << "\n";
    	//std::cout << " # " << typeid(T).name() << " has " << (shared_from_this().use_count() - 1) << " instances." << "\n";
		return std::dynamic_pointer_cast<T>(shared_from_this());
	}

	template <class T>
	std::shared_ptr<const T> dynamic_shared_this() const {
    	//std::cout << "## Debug Info : " << typeid(T).name() << " created with dynamic_shared_this()." << "\n";
    	//std::cout << " # " << typeid(T).name() << " has " << (shared_from_this().use_count() - 1) << " instances." << "\n";
		return std::dynamic_pointer_cast<const T>(shared_from_this());
	}
};

#endif
