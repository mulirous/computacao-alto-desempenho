## Compilação na GPU

### Compilação

```shell
nvc -mp=gpu -o heat heat.c
```

## Compilação CUDA

```shell
nvcc -o meu_programa meu_programa.cu
```

## Uso do nsys

### Seu uso em qualquer executável

```shell
nsys ./executavel
```

### Visualização de arquivo nsys

```shell
nsys stats nsys-[arquivo].nsys-rep
```
