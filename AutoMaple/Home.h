#pragma once

namespace AutoMaple {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for Home
	/// </summary>
	public ref class Home : public System::Windows::Forms::Form
	{
	public:
		Home(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Home()
		{
			if (components)
			{
				delete components;
			}
		}


	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;
	private: System::Windows::Forms::Timer^  timer1;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel2;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  button3;
	private: System::Windows::Forms::CheckBox^  checkBox1;
	private: System::Windows::Forms::ListBox^  listBox1;
	private: System::Windows::Forms::CheckBox^  checkBox2;

	protected:


	private: System::ComponentModel::IContainer^  components;
	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->checkBox2 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->tableLayoutPanel2 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->listBox1 = (gcnew System::Windows::Forms::ListBox());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->tableLayoutPanel1->SuspendLayout();
			this->tableLayoutPanel2->SuspendLayout();
			this->SuspendLayout();
			// 
			// tableLayoutPanel1
			// 
			this->tableLayoutPanel1->ColumnCount = 1;
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle()));
			this->tableLayoutPanel1->Controls->Add(this->checkBox2, 0, 7);
			this->tableLayoutPanel1->Controls->Add(this->checkBox1, 0, 4);
			this->tableLayoutPanel1->Controls->Add(this->button3, 0, 1);
			this->tableLayoutPanel1->Controls->Add(this->button1, 0, 3);
			this->tableLayoutPanel1->Controls->Add(this->button2, 0, 2);
			this->tableLayoutPanel1->Controls->Add(this->tableLayoutPanel2, 0, 0);
			this->tableLayoutPanel1->Controls->Add(this->listBox1, 0, 8);
			this->tableLayoutPanel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel1->Location = System::Drawing::Point(0, 0);
			this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
			this->tableLayoutPanel1->RowCount = 9;
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 25)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 20)));
			this->tableLayoutPanel1->Size = System::Drawing::Size(404, 381);
			this->tableLayoutPanel1->TabIndex = 2;
			// 
			// checkBox2
			// 
			this->checkBox2->AutoSize = true;
			this->checkBox2->Checked = true;
			this->checkBox2->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox2->Location = System::Drawing::Point(3, 193);
			this->checkBox2->Name = L"checkBox2";
			this->checkBox2->Size = System::Drawing::Size(140, 17);
			this->checkBox2->TabIndex = 3;
			this->checkBox2->Text = L"Log maple function calls";
			this->checkBox2->UseVisualStyleBackColor = true;
			this->checkBox2->CheckedChanged += gcnew System::EventHandler(this, &Home::checkBox2_CheckedChanged);
			// 
			// checkBox1
			// 
			this->checkBox1->AutoSize = true;
			this->checkBox1->Location = System::Drawing::Point(3, 168);
			this->checkBox1->Name = L"checkBox1";
			this->checkBox1->Size = System::Drawing::Size(196, 17);
			this->checkBox1->TabIndex = 3;
			this->checkBox1->Text = L"Dock left of MS window (may crash)";
			this->checkBox1->UseVisualStyleBackColor = true;
			this->checkBox1->CheckedChanged += gcnew System::EventHandler(this, &Home::checkBox1_CheckedChanged);
			// 
			// button3
			// 
			this->button3->AutoSize = true;
			this->button3->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->button3->Dock = System::Windows::Forms::DockStyle::Fill;
			this->button3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->button3->Location = System::Drawing::Point(3, 60);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(398, 30);
			this->button3->TabIndex = 5;
			this->button3->Text = L"Open file in default program";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &Home::button3_Click);
			// 
			// button1
			// 
			this->button1->AutoSize = true;
			this->button1->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->button1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->button1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->button1->Location = System::Drawing::Point(3, 132);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(398, 30);
			this->button1->TabIndex = 3;
			this->button1->Text = L"Run file from the MapleStory folder";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &Home::button1_Click);
			// 
			// button2
			// 
			this->button2->AutoSize = true;
			this->button2->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->button2->Dock = System::Windows::Forms::DockStyle::Fill;
			this->button2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->button2->Location = System::Drawing::Point(3, 96);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(398, 30);
			this->button2->TabIndex = 2;
			this->button2->Text = L"Stop script";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &Home::button2_Click);
			// 
			// tableLayoutPanel2
			// 
			this->tableLayoutPanel2->ColumnCount = 1;
			this->tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle()));
			this->tableLayoutPanel2->Controls->Add(this->textBox1, 0, 1);
			this->tableLayoutPanel2->Controls->Add(this->label1, 0, 0);
			this->tableLayoutPanel2->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel2->Location = System::Drawing::Point(3, 3);
			this->tableLayoutPanel2->Name = L"tableLayoutPanel2";
			this->tableLayoutPanel2->RowCount = 2;
			this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle()));
			this->tableLayoutPanel2->Size = System::Drawing::Size(398, 51);
			this->tableLayoutPanel2->TabIndex = 4;
			// 
			// textBox1
			// 
			this->textBox1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->textBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->textBox1->Location = System::Drawing::Point(3, 23);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(392, 26);
			this->textBox1->TabIndex = 5;
			this->textBox1->Text = L"test.lua";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(3, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(271, 20);
			this->label1->TabIndex = 0;
			this->label1->Text = L"File to load from maplestory directory:";
			// 
			// listBox1
			// 
			this->listBox1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->listBox1->FormattingEnabled = true;
			this->listBox1->Location = System::Drawing::Point(3, 216);
			this->listBox1->Name = L"listBox1";
			this->listBox1->Size = System::Drawing::Size(398, 162);
			this->listBox1->TabIndex = 3;
			// 
			// timer1
			// 
			this->timer1->Interval = 50;
			this->timer1->Tick += gcnew System::EventHandler(this, &Home::timer1_Tick);
			// 
			// Home
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(404, 381);
			this->Controls->Add(this->tableLayoutPanel1);
			this->MinimumSize = System::Drawing::Size(420, 420);
			this->Name = L"Home";
			this->Text = L"Home";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &Home::Home_FormClosing);
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &Home::Home_FormClosed);
			this->Load += gcnew System::EventHandler(this, &Home::Home_Load);
			this->tableLayoutPanel1->ResumeLayout(false);
			this->tableLayoutPanel1->PerformLayout();
			this->tableLayoutPanel2->ResumeLayout(false);
			this->tableLayoutPanel2->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
		private: System::Void Home_Load(System::Object^  sender, System::EventArgs^  e);
		private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void Home_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e);
		private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void Home_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e);
		private: System::Void timer1_Tick(System::Object^  sender, System::EventArgs^  e);
		private: System::Void button3_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void checkBox1_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		public: System::Void Home::Log(const char * c);
		private: System::Void checkBox2_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
};
}
