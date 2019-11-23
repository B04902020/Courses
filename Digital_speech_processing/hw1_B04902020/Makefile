all: 
	g++ train.cpp -o train
	g++ test.cpp -o test
	g++ acc.cpp -o acc
train:
	./train 850 model_init.txt seq_model_01.txt model_01.txt
	./train 850 model_init.txt seq_model_02.txt model_02.txt
	./train 850 model_init.txt seq_model_03.txt model_03.txt
	./train 850 model_init.txt seq_model_04.txt model_04.txt
	./train 850 model_init.txt seq_model_05.txt model_05.txt
test_model:	
	./test modellist.txt testing_data1.txt result1.txt
	./test modellist.txt testing_data2.txt result2.txt
	./acc result1.txt testing_answer.txt > acc.txt
	cat acc.txt
clean:
	rm -rf train
	rm -rf test
	rm -rf acc
