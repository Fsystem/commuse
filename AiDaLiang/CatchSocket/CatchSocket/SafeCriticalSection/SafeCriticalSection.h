#pragma once

namespace COMMUSE
{
	/*****************************************************************************
	* 类  名：CCriticalSection
	* 功  能：锁类
	* 作  者：双刃剑
	* 日  期：2010-12-29 9:21:12
	******************************************************************************/
	#define LOCK(cs) CCritiSection::CLock lk(cs)

	class CCritiSection
	{
	public:
		class CLock
		{
		public:

			CLock(CCritiSection& cs):_crit(cs){  _crit.Enter();  };

			virtual ~CLock(){  _crit.Leave();  };

		protected:
		private:
			CCritiSection& _crit;
		};

		CCritiSection();

		virtual ~CCritiSection();

		void Enter(){ EnterCriticalSection(&crit); };

		void Leave(){ LeaveCriticalSection(&crit); };

	protected:
	private:
		CRITICAL_SECTION crit;
	};



}