#include "CommandHandler.hpp"

namespace AxxonsoftInternProject
{
	namespace Client
	{
		CommandHandler::CommandHandler(std::shared_ptr<http::HTTPRequest> outputRequest) :
			m_outputRequest{ outputRequest }
		{

		}

		void CommandHandler::setHeaders()
		{
			if (m_outputRequest->m_body.size() != 0)
			{
				m_outputRequest->m_headers.push_back
				(
					http::HTTPHeader
					{ 
						AxxonsoftInternProject::http::stock::headers::names::g_contentLength, 
						std::to_string(m_outputRequest->m_body.size()) 
					}
				);
			}
		}

		void CommandHandler::extructTargetIntoRequestBody(std::string target)
		{
			std::string temp = "";
			std::string path = "";
			bool isFilename = false;
			m_requestBody[AxxonsoftInternProject::http::stock::json::g_pathFileldName] = "";
			m_requestBody[AxxonsoftInternProject::http::stock::json::g_filenameFiledName] = "";

			for (auto character : target)
			{
				if (character == '/')
				{
					if (isFilename)
					{
						throw new Client::InvalidTargetException;
					}
					else
					{
						path += temp;
						temp = "";
					}
				}
				else if (character == '.')
				{
					isFilename = true;
				}

				temp.push_back(character);
			}

			if (isFilename)
			{
				m_requestBody[AxxonsoftInternProject::http::stock::json::g_pathFileldName] = temp;
			}
			else
			{
				path += temp;
			}

			m_requestBody[AxxonsoftInternProject::http::stock::json::g_pathFileldName] = path;
		}

		void CommandHandler::putFileDataInRequestBody(std::ifstream& file)
		{
			auto end = file.tellg();
			file.seekg(0, std::ios::beg);

			auto size = size_t(end - file.tellg());
			std::vector<std::byte> buffer(size);

			file.read((char*)buffer.data(), buffer.size());

			m_requestBody[AxxonsoftInternProject::http::stock::json::g_dataFieldName] = buffer;
		}

		void CommandHandler::getFileData(std::ifstream& file)
		{
			std::string filePath;
			std::string filename;

			putFileDataInRequestBody(file);

			std::cout << AxxonsoftInternProject::http::stock::messages::g_inputFilePathOnServer << "\n";

			if (std::getline(std::cin, filePath))
			{
				m_requestBody[AxxonsoftInternProject::http::stock::json::g_pathFileldName] = filePath;
			}

			std::cout << AxxonsoftInternProject::http::stock::messages::g_inputFileNameOnServer << "\n";

			if (std::getline(std::cin, filename))
			{
				m_requestBody[AxxonsoftInternProject::http::stock::json::g_filenameFiledName] = filename;
			}
		}

		void CommandHandler::handlePostCommand()
		{
			setRequestUriAndMethod
			( 
				AxxonsoftInternProject::http::stock::requestMethods::g_POST, 
				AxxonsoftInternProject::http::stock::uri::g_empty, 
				http::ClientRequestType::sendTarget
			);
			std::string pathToPostedFile = m_comand.m_targer;

			if (std::filesystem::exists(pathToPostedFile))
			{
				std::ifstream file(pathToPostedFile, std::ios::binary | std::ios::ate);

				if (file.is_open())
				{
					getFileData(file);

					file.close();
				}
				else
				{
					file.close();
					throw new Client::CantOpenPostedFileException;
				}
			}
			else
			{
				throw new Client::NoFileToPostException;
			}
		}

		void CommandHandler::setRequestUriAndMethod(const std::string& method, const std::string& uri, const http::ClientRequestType& type)
		{
			m_outputRequest->m_method = method;
			m_outputRequest->m_uri = uri;
			m_outputRequest->m_type = type;
		}

		void CommandHandler::handleGetCommand()
		{
			setRequestUriAndMethod(
				AxxonsoftInternProject::http::stock::requestMethods::g_GET,
				AxxonsoftInternProject::http::stock::uri::g_empty,
				http::ClientRequestType::downloadTarget);
		}

		void CommandHandler::handleLSCommand()
		{
			setRequestUriAndMethod(
				AxxonsoftInternProject::http::stock::requestMethods::g_GET,
				AxxonsoftInternProject::http::stock::uri::g_getContent,
				http::ClientRequestType::checkTarget);
		}

		void CommandHandler::handleDeleteCommand()
		{
			setRequestUriAndMethod(
				AxxonsoftInternProject::http::stock::requestMethods::g_DELETE,
				AxxonsoftInternProject::http::stock::uri::g_empty,
				http::ClientRequestType::deleteTarget);
		}

		void CommandHandler::handleCommand()
		{
			try
			{
				if (this->m_comand.m_command != AxxonsoftInternProject::http::stock::clientCommands::g_post)
				{
					this->extructTargetIntoRequestBody(this->m_comand.m_targer);

					if (this->m_comand.m_command == AxxonsoftInternProject::http::stock::clientCommands::g_ls)
					{
						this->handleLSCommand();
					}
					else if (this->m_comand.m_command == AxxonsoftInternProject::http::stock::clientCommands::g_get)
					{
						this->handleGetCommand();
					}
					else if(this->m_comand.m_command == AxxonsoftInternProject::http::stock::clientCommands::g_delete)
					{
						this->handleDeleteCommand();
					}
					else
					{
						std::cout << AxxonsoftInternProject::http::stock::messages::g_invalidMethod << "\n";
					}
				}
				else
				{
					handlePostCommand();
				}
			}
			catch (std::exception& ex)
			{
				std::cout << ex.what() << "\n";
			}
			catch (boost::exception& ex)
			{
				std::cout << boost::diagnostic_information(ex) << "\n";
			}
		}

		void CommandHandler::Handle(Command command)
		{
			this->m_comand = command;

			this->m_outputRequest->m_version = config::g_httpVersion;
			this->handleCommand();
			this->m_outputRequest->m_body = this->m_requestBody.dump(4);
			this->setHeaders();
		}
	}
}