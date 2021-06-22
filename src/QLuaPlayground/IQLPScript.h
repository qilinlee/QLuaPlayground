#pragma once

namespace QLP
{
	class IQLPScript
	{
	public:
		virtual ~IQLPScript()
		{
		}
		virtual void RegisteLuaEnv() = 0;
		virtual void OnDestroyed() = 0;
	};
}
