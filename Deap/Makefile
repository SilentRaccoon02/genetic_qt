protoc:
	python -m grpc_tools.protoc --proto_path=../GeneticQt/ --python_out=. --grpc_python_out=. ../GeneticQt/common.proto
install:
	pyinstaller -F main.py
