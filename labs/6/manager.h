#ifndef MANAGER
#define MANAGER
#include <unistd.h>
int has_initialized = 0;

void *managed_memory_start;

void *last_valid_address;

struct mem_control_block {
int is_available;
int size;
};

void malloc_init()

{

//запрашиваем у ОС последний валидный адресс

last_valid_address = sbrk(0);
managed_memory_start = last_valid_address;
has_initialized = 1;

}

void free(void*firstbyte) {

struct mem_control_block *mcb;

/* находим mem_control_block и восстанавливаем структуру */

mcb = (mem_control_block *)(firstbyte - sizeof(struct mem_control_block));
mcb->is_available = 1;
return;

}



void *malloc(long numbytes) {

    /* Запоминаем текущую область памяти, где мы ищем */

    void *current_location;
    struct mem_control_block * current_location_mcb;
    void *memory_location;

    if(! has_initialized) {

        malloc_init();

    }

    /* Память, которую мы ищем, должна включать память для хранения блока управления памятью (memory control block).
       пользователям функции malloc не обязательно знать об этом, поэтому мы просто скрываем это от них.*/

    numbytes = numbytes + sizeof(struct mem_control_block);
    memory_location = 0;
    current_location = managed_memory_start;

    /* Продолжаем это делать, пока не просмотрим все доступное пространство */

    while(current_location != last_valid_address)
    {

        /* current_location и current_location_mcb указывают на один и тот же адрес. Так как current_location_mcb имеет соответствующий тип,
         * то мы можем его использовать в качестве структуры.
         *  current_location это свободный указатель (void pointer), поэтому мы можем его использовать, чтобы просчитать адресацию. */

        current_location_mcb = (struct mem_control_block *)current_location;
        if(current_location_mcb->is_available)
        {
            if(current_location_mcb->size >= numbytes)
            {
                /* нашли не занятое место подходящего размера.*/
                current_location_mcb->is_available = 0;
                memory_location = current_location;
                break;
            }
        }
        // Текущий блок памяти не подходит, переходим к следующему
        current_location = current_location + current_location_mcb->size;
    }
    /* Если не удалось найти подходящее место в памяти, просим у операционной системы еще памяти */

    if(! memory_location)

    {
        /* Сдвигаем границу на заданное число байт вверх */
        if (sbrk(numbytes) == (void*)-1) return 0; //ошибка, память не выделена
        memory_location = last_valid_address;
        last_valid_address = last_valid_address + numbytes;
        current_location_mcb =(mem_control_block *) memory_location;
        current_location_mcb->is_available = 0;
        current_location_mcb->size = numbytes;
    }

    //memory_location адресует память, включая блок mem_control_block

    memory_location = memory_location + sizeof(struct mem_control_block);

return memory_location;

}

void * realloc (void * memory_location,long numbytes)
{
    void * control_block_location = memory_location - sizeof(struct mem_control_block);
    mem_control_block * control_block = (struct mem_control_block *) control_block_location;
    if (numbytes < control_block->size) return 0;
    void * new_memory = malloc(numbytes);
    for (int i=0;i<control_block->size;i++)
        ((char*)new_memory)[i]=((char*)memory_location)[i];
    free(memory_location);
    return new_memory;
}

#endif // MANAGER

