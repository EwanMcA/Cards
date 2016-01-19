#pragma once

namespace BoizStone {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
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
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ToolStripMenuItem^  fileMenu;
	protected:
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem2;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem3;
	private: System::Windows::Forms::ToolStripMenuItem^  gameToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  buildDeckToolStripMenuItem;
	private: System::Windows::Forms::MenuStrip^  menuStrip1;

	private: System::Windows::Forms::PictureBox^  pictureBox1;
	private: System::Windows::Forms::Button^  imageButton;
	private: System::Windows::Forms::PictureBox^  pictureBoxBG;
	private: System::Windows::Forms::FlowLayoutPanel^  featuresPanel;
	private: System::Windows::Forms::FlowLayoutPanel^  chosenFeaturesPanel;





	protected:







	protected:

	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MyForm::typeid));
			this->fileMenu = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem2 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem3 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->gameToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->buildDeckToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->imageButton = (gcnew System::Windows::Forms::Button());
			this->pictureBoxBG = (gcnew System::Windows::Forms::PictureBox());
			this->featuresPanel = (gcnew System::Windows::Forms::FlowLayoutPanel());
			this->chosenFeaturesPanel = (gcnew System::Windows::Forms::FlowLayoutPanel());
			this->menuStrip1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBoxBG))->BeginInit();
			this->SuspendLayout();
			// 
			// fileMenu
			// 
			this->fileMenu->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
				this->toolStripMenuItem2,
					this->toolStripMenuItem3
			});
			this->fileMenu->Name = L"fileMenu";
			this->fileMenu->Size = System::Drawing::Size(50, 29);
			this->fileMenu->Text = L"File";
			this->fileMenu->Click += gcnew System::EventHandler(this, &MyForm::toolStripMenuItem1_Click);
			// 
			// toolStripMenuItem2
			// 
			this->toolStripMenuItem2->Name = L"toolStripMenuItem2";
			this->toolStripMenuItem2->Size = System::Drawing::Size(141, 30);
			this->toolStripMenuItem2->Text = L"Open";
			// 
			// toolStripMenuItem3
			// 
			this->toolStripMenuItem3->Name = L"toolStripMenuItem3";
			this->toolStripMenuItem3->Size = System::Drawing::Size(141, 30);
			this->toolStripMenuItem3->Text = L"Save";
			// 
			// gameToolStripMenuItem
			// 
			this->gameToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->buildDeckToolStripMenuItem });
			this->gameToolStripMenuItem->Name = L"gameToolStripMenuItem";
			this->gameToolStripMenuItem->Size = System::Drawing::Size(70, 29);
			this->gameToolStripMenuItem->Text = L"Game";
			// 
			// buildDeckToolStripMenuItem
			// 
			this->buildDeckToolStripMenuItem->Name = L"buildDeckToolStripMenuItem";
			this->buildDeckToolStripMenuItem->Size = System::Drawing::Size(180, 30);
			this->buildDeckToolStripMenuItem->Text = L"Build Deck";
			// 
			// menuStrip1
			// 
			this->menuStrip1->ImageScalingSize = System::Drawing::Size(24, 24);
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) { this->fileMenu, this->gameToolStripMenuItem });
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Padding = System::Windows::Forms::Padding(6, 3, 0, 3);
			this->menuStrip1->Size = System::Drawing::Size(1361, 35);
			this->menuStrip1->TabIndex = 0;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// pictureBox1
			// 
			this->pictureBox1->BackColor = System::Drawing::Color::Transparent;
			this->pictureBox1->Location = System::Drawing::Point(952, 136);
			this->pictureBox1->Margin = System::Windows::Forms::Padding(2, 3, 2, 3);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(195, 245);
			this->pictureBox1->TabIndex = 1;
			this->pictureBox1->TabStop = false;
			// 
			// imageButton
			// 
			this->imageButton->Font = (gcnew System::Drawing::Font(L"Raavi", 8, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->imageButton->ForeColor = System::Drawing::SystemColors::ActiveCaptionText;
			this->imageButton->Location = System::Drawing::Point(934, 80);
			this->imageButton->Margin = System::Windows::Forms::Padding(2, 3, 2, 3);
			this->imageButton->Name = L"imageButton";
			this->imageButton->Size = System::Drawing::Size(119, 29);
			this->imageButton->TabIndex = 2;
			this->imageButton->Text = L"Choose Image";
			this->imageButton->UseVisualStyleBackColor = true;
			this->imageButton->Click += gcnew System::EventHandler(this, &MyForm::imageButton_Click);
			// 
			// pictureBoxBG
			// 
			this->pictureBoxBG->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBoxBG.Image")));
			this->pictureBoxBG->Location = System::Drawing::Point(743, 115);
			this->pictureBoxBG->Name = L"pictureBoxBG";
			this->pictureBoxBG->Size = System::Drawing::Size(618, 353);
			this->pictureBoxBG->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBoxBG->TabIndex = 3;
			this->pictureBoxBG->TabStop = false;
			// 
			// featuresPanel
			// 
			this->featuresPanel->AutoScroll = true;
			this->featuresPanel->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(192)),
				static_cast<System::Int32>(static_cast<System::Byte>(128)));
			this->featuresPanel->FlowDirection = System::Windows::Forms::FlowDirection::TopDown;
			this->featuresPanel->Location = System::Drawing::Point(54, 80);
			this->featuresPanel->Name = L"featuresPanel";
			this->featuresPanel->Size = System::Drawing::Size(194, 407);
			this->featuresPanel->TabIndex = 4;
			// 
			// chosenFeaturesPanel
			// 
			this->chosenFeaturesPanel->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)),
				static_cast<System::Int32>(static_cast<System::Byte>(192)), static_cast<System::Int32>(static_cast<System::Byte>(128)));
			this->chosenFeaturesPanel->Location = System::Drawing::Point(442, 80);
			this->chosenFeaturesPanel->Name = L"chosenFeaturesPanel";
			this->chosenFeaturesPanel->Size = System::Drawing::Size(200, 407);
			this->chosenFeaturesPanel->TabIndex = 5;
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 27);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(192)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->ClientSize = System::Drawing::Size(1361, 661);
			this->Controls->Add(this->chosenFeaturesPanel);
			this->Controls->Add(this->featuresPanel);
			this->Controls->Add(this->imageButton);
			this->Controls->Add(this->pictureBox1);
			this->Controls->Add(this->menuStrip1);
			this->Controls->Add(this->pictureBoxBG);
			this->Font = (gcnew System::Drawing::Font(L"Raavi", 8, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ForeColor = System::Drawing::SystemColors::ActiveBorder;
			this->Margin = System::Windows::Forms::Padding(2, 5, 2, 5);
			this->Name = L"MyForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"BoizStone";
			this->WindowState = System::Windows::Forms::FormWindowState::Maximized;
			this->Load += gcnew System::EventHandler(this, &MyForm::MyForm_Load);
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBoxBG))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void splitContainer1_Panel1_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {
	}
	private: System::Void toolStripMenuItem1_Click(System::Object^  sender, System::EventArgs^  e) {
	}
	private: System::Void MyForm_Load(System::Object^  sender, System::EventArgs^  e) {
	}
private: System::Void openFileDialog1_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) {
}
private: System::Void imageButton_Click(System::Object^  sender, System::EventArgs^  e) {
	// Displays an OpenFileDialog so the user can select a Cursor.
	OpenFileDialog ^ openFileDialog1 = gcnew OpenFileDialog();
	openFileDialog1->Filter = "Image Files|*.png";
	openFileDialog1->Title = "Select an image";

	// Show the Dialog.
	// If the user clicked OK in the dialog and
	// a file was selected, open it.
	if (openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
	{
		//pictureBox1->BackgroundImage(openFileDialog1->OpenFile());
		pictureBox1->SizeMode = PictureBoxSizeMode::Zoom;
		pictureBox1->Image = Image::FromFile(openFileDialog1->FileName);
	}
}
};
}
