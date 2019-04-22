#include<stdio.h>
#include<stdlib.h>
#include <sys/time.h>

typedef struct item_F{
	//item struct for fractional knapsack problem
	int benefit;
	int weight;
	float BperW;
}item_F;
typedef struct Node{
	int level;
	int benefit;
	int weight;
	float bound;
	struct Node *next;
}Node, *NodePtr;

typedef struct Queue{
	NodePtr top;
	NodePtr tail;
}Qtype, *Queue;
struct timeval time1, time2;
Queue initQ();
int isEmpty(Queue q);
void EnQ(Queue q, int level, int benefit, int weight, float bound);
void DeQ(Queue q, NodePtr temp);
int BnB(int itemNum, int * benefit, int * weight);
float greedy(int itemNum, struct item_F * items);
int dynamic(int itemNum, int * benefit, int * weight);
float get_diff(struct timeval time1, struct timeval time2);
void swap(item_F * A, item_F * B);
int partition (struct item_F * arr, int low, int high);
void quickSort(struct item_F * arr, int low, int high);
void generateItems(struct item_F * items, int itemNum);
int main(){
	item_F * items;
	int inputs[9]={10, 100, 500, 1000, 3000, 5000, 7000, 9000, 10000};
    int index, i, itemNum;
    int *benefit;
	int *weight;
    float total_benefit_greedy;
    int total_benefit_dynamic;
    int total_benefit_BB;
    FILE * fp;
    fp =fopen("output.txt", "a");
    fprintf(fp, "Number of | Processing time in milliseconds/ Maximun benefit value|\n");
    fprintf(fp, "%10s %35s% 25s% 25s%\n","items","Greedy","Dynamic", "B.B");
    for(index=0;index<sizeof(inputs)/sizeof(int);index++){
    	itemNum =inputs[index];
    	items = (item_F *)malloc(sizeof(item_F)*itemNum);
    	for(i=0;i<itemNum;i++){
		    items[i].weight = rand() % 100 + 1;
		    items[i].benefit = rand() % 300 + 1;
		    items[i].BperW = (float)items[i].benefit/items[i].weight;
    	}
    	benefit = (int*)malloc(sizeof(int)*itemNum);
		weight = (int*)malloc(sizeof(int)*itemNum);
    	for(i=0;i<itemNum;i++){
    		benefit[i]=items[i].benefit;
    		weight[i]=items[i].weight;
		}
    	gettimeofday(&time1, 0);
    	total_benefit_greedy = greedy(itemNum, items);
    	printf("Benefit of %d inputs: %f Time take: %f\n", itemNum, total_benefit_greedy, get_diff(time1, time2));
    	fprintf(fp, "%10d %20f / %10.3f  ",itemNum, total_benefit_greedy, get_diff(time1, time2));
    	
    	gettimeofday(&time1, 0);
    	total_benefit_dynamic = dynamic(inputs[index], benefit, weight);
    	printf("Benefit of %d inputs: %d Time take: %f\n", inputs[index], total_benefit_dynamic, get_diff(time1, time2));
    	fprintf(fp, "%10d / %10.3f  ", total_benefit_dynamic, get_diff(time1, time2));
		
    	for(i=0;i<itemNum;i++){
    		benefit[i]=items[i].benefit;
    		weight[i]=items[i].weight;
		}
		gettimeofday(&time1, 0);
		total_benefit_BB = BnB(inputs[index], benefit, weight);
		printf("Benefit of %d inputs: %d Time take: %f\n", inputs[index], total_benefit_BB, get_diff(time1, time2));
		fprintf(fp, "%10d / %10.3f  \n", total_benefit_BB, get_diff(time1, time2));
		
		free(benefit);
		free(weight);
		free(items);

	}
	fclose(fp);
    return 0;
}
int BnB(int itemNum, int * benefit, int * weight){
	const int Weight = itemNum*40;
	int Max_benefit, level ,i, node_benefit, node_weight;
	int cur_weight= Weight;
	float cur_bound=0;
	Max_benefit=0;
	level=-1;
//	printf("Starting Branch and Bound\n");
	Queue q = initQ();
	for(i=0;i<itemNum;i++){
    	cur_weight-=weight[i];
    	if(cur_weight>=0)
    	cur_bound+=benefit[i];
    	else{
    	cur_weight += weight[i];
    	cur_bound += (float)cur_weight*((float)benefit[i]/weight[i]);
		break;
		}
	}
	EnQ(q, -1, 0, 0, cur_bound);
//	printf("root node successfully generated\n");
	NodePtr temp = malloc(sizeof(Queue));
	while(!isEmpty(q)){
		DeQ(q, temp);
		if(temp->level>=itemNum||temp->bound<Max_benefit||temp->weight>Weight){

		}else{
		level = temp->level+1;
		//when we decide to choose the item.
		node_weight = temp->weight+weight[level];
		node_benefit = temp->benefit+benefit[level]; 
		if(node_weight<=Weight){
			if(Max_benefit<node_benefit){
				Max_benefit = node_benefit;
			}
		}
		cur_weight= Weight-node_weight;
		cur_bound = (float)node_benefit;
		for(i=level+1;i<itemNum;i++){
			cur_weight-=weight[i];
    		if(cur_weight>0){
    			cur_bound+=benefit[i];
			}else if(cur_weight==0){
				cur_bound+=benefit[i];
				break;
			}else{
    		cur_weight += weight[i];
    		cur_bound += (float)cur_weight*((float)benefit[i]/weight[i]);
			break;
			}
		}
		EnQ(q, level, node_benefit, node_weight, cur_bound);


		//when we decide not to choose the item.
		node_weight = temp->weight;
		node_benefit = temp->benefit; 
		cur_weight= Weight-node_weight;
		cur_bound = (float)node_benefit;
		for(i=level+1;i<itemNum;i++){
			cur_weight-=weight[i];
    		if(cur_weight>=0){
    			cur_bound+=benefit[i];
			}
    		else{
    		cur_weight += weight[i];
    		cur_bound += (float)cur_weight*((float)benefit[i]/weight[i]);
			break;
			}
		}
		EnQ(q, level, node_benefit, node_weight, cur_bound);
		}
		gettimeofday(&time2, 0);
		if(get_diff(time1, time2)>=(15*60*1000)){
			return -999;
		}
	}

    return Max_benefit;
}
Queue initQ(){
	Queue newQ = malloc(sizeof(Qtype));
	newQ->top = NULL;
	newQ->tail = NULL;
	return newQ;
}
int isEmpty(Queue q){
	return q->top==NULL;
}

void EnQ(Queue q, int level, int benefit, int weight, float bound){
	NodePtr newNode = malloc(sizeof(Node));
	if(newNode==NULL){
		printf("Failed to create new Node!\n");
	}
	newNode->level = level;
	newNode->bound = bound;
	newNode->benefit = benefit;
	newNode->weight = weight;
	newNode->next = NULL;
	if(!isEmpty(q)){
		if(newNode->bound>q->top->bound){
			newNode->next = q->top;
			q->top = newNode;
		}else{
			NodePtr prevNode = q->top;
			NodePtr temp = q->top->next;
			while(temp!=NULL&&temp->bound > newNode->bound){
				prevNode = temp;
				temp = temp->next;
			}
			if(temp==NULL){
				prevNode->next = newNode; 
				q->tail=newNode;
			}else{
				newNode->next = temp;
				prevNode->next = newNode;
			}
		}
	}else{
		q->top=newNode;
		q->tail=newNode;
	}
}
void DeQ(Queue q, NodePtr temp){
//	printf("DeQ");
	if(isEmpty(q)){
		printf("EmptyQueue!");
		return;
	}
	NodePtr temp2 = q->top;
	temp->bound = temp2->bound;
	temp->level = temp2->level;
	temp->benefit = temp2->benefit;
	temp->weight = temp2->weight;
	q->top=q->top->next;
	free(temp2);
}
int dynamic(int itemNum, int * benefit, int * weight){
	int Weight = itemNum*40;
	int w, wofi, i, Benefit, item;
	int **B=malloc(sizeof(int*)*(itemNum+1));
	if(B==NULL){
		fprintf(stderr, "out of memory for B\n");
	}
	for(i=0;i<itemNum+1;i++){
		B[i]=malloc((Weight+1)*sizeof(int));
		if(B[i]==NULL){
		fprintf(stderr, "out of memory for B[%d]\n",i);
		gettimeofday(&time2, 0);
		return -100;
		}

	}

	for(w=0;w<=Weight; w++)
		B[0][w]=0;

	for(i=1;i<=itemNum;i++){
		B[i][0]=0;
		item = i-1;
		for(w=1;w<=Weight; w++){
			if(weight[item]<=w){
				wofi=weight[item];
				if(benefit[item]+B[i-1][w-wofi]>B[i-1][w])
					B[i][w]= benefit[item] + B[i-1][w-wofi];
				else B[i][w]=B[i-1][w];
			}else B[i][w]=B[i-1][w];
		}
		gettimeofday(&time2, 0);
		if(get_diff(time1, time2)>=(15*60*1000)){
			return -999;
		}
	}
	Benefit =B[itemNum][Weight];	
	for (i = 0; i < itemNum+1; i++)
	{
	    int* currentIntPtr = B[i];
	    free(currentIntPtr);
	}

	gettimeofday(&time2, 0);
	return Benefit;
}
float get_diff(struct timeval time1, struct timeval time2)
{
     return (time2.tv_sec - time1.tv_sec) * 1000.0f + (time2.tv_usec - time1.tv_usec) / 1000.0f;
}
void swap(item_F * A, item_F * B){
	struct item_F temp;
	temp.benefit = A->benefit;
	temp.weight = A->weight;
	temp.BperW = A->BperW;
	
	A->benefit = B->benefit;
	A->weight = B->weight;
	A->BperW = B->BperW;
	
	B->benefit = temp.benefit;
	B->weight = temp.weight;
	B->BperW = temp.BperW;
}
int partition (struct item_F * arr, int low, int high) 
{ 
    float pivot = arr[high].BperW;    // pivot 
    int i = (low - 1);  // Index of smaller element 
  	int j;
    for (j = low; j <= high- 1; j++) 
    { 
        // If current element is smaller than or 
        // equal to pivot 
        if (arr[j].BperW >= pivot) 
        { 
            i++;    // increment index of smaller element 
            swap(&arr[i], &arr[j]); 
        } 
    } 
    swap(&arr[i + 1], &arr[high]); 
    return (i + 1); 
}
void quickSort(struct item_F * arr, int low, int high) 
{ 
    if (low < high) 
    { 
        /* pi is partitioning index, arr[p] is now 
           at right place */
        int pi = partition(arr, low, high); 
  
        // Separately sort elements before 
        // partition and after partition 
        quickSort(arr, low, pi - 1); 
        quickSort(arr, pi + 1, high); 
    } 
}  
float greedy(int itemNum, struct item_F * items){
    int Weight = itemNum*40;
    int i;
	float Benefit;
//	for(i=0;i<itemNum;i++){
//		printf("benefit: %d, weight: %d, BperW: %f\n", items[i].benefit, items[i].weight, items[i].BperW);
//	}
    quickSort(items, 0, itemNum-1);
    
    for(i=0;i<itemNum;i++){
    	Weight-=items[i].weight;
    	if(Weight>=0)
    	Benefit+=items[i].benefit;
    	else{
    	Weight += items[i].weight;
    	Benefit += (float)Weight*items[i].BperW;
		break;
		}
		gettimeofday(&time2, 0);
		if(get_diff(time1, time2)>=(15*60*1000)){
			return -999;
		}
	}
    return Benefit;
}
