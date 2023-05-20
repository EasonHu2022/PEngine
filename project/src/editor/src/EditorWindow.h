namespace pengine
{
	class EditorWindow
	{
	public:
		virtual ~EditorWindow() = default;
		virtual void onImGui() = 0;

	};


}