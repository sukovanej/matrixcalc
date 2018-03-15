#include "eval.h"

//
// Created by Milan Suk on 15.03.18.
//

/**
 * Evaluate file input
 * @param input
 * @param env
 */
void eval(FILE* input, Environment *env) {
    AbstractSyntaxTreeNode* node = ast_parse(input);

    while (node->token.state != STATE_END) {
        eval_ast(node, env);
        node = ast_parse(input);
    }
}


/**
 * Evaluate AST
 * @param file
 * @param env
 */
void eval_ast(AbstractSyntaxTreeNode *file, Environment *env) {
    Matrix* m;

    switch (file->token.state) {
        case STATE_EQUAL:
            eval_set_variable(file->left->token.value, file->right, env);
            break;
        default:
            m = eval_matrix(file, env);
            matrix_print(m);
            break;
    }
}

Matrix* eval_matrix(AbstractSyntaxTreeNode *value, Environment *env) {
    Matrix* matrix = matrix_create_new(1, 1);
    AbstractSyntaxTreeNode* node = value;

    if (node->token.state != STATE_DELIMITER && node->token.state != STATE_NEWLINE_DELIMITER) {
        matrix = eval_ops(value, env);
    } else if (node->token.state == STATE_DELIMITER) {
        size_t i = 0;

        while (node->token.state == STATE_DELIMITER) {
            matrix->columns++;
            matrix->matrix[0] = realloc(matrix->matrix[0], sizeof(double) * matrix->columns);
            matrix->matrix[0][i] = eval_ops(node->left, env)->matrix[0][0];

            i++;
            node = node->right;
        }

        matrix->matrix[0][i] = eval_ops(node, env)->matrix[0][0];
    } else {
        size_t i = 0;

        while (node->token.state == STATE_NEWLINE_DELIMITER) {
            Matrix* m = eval_matrix(node->left, env);
            matrix->rows++;
            matrix->columns = m->columns;
            matrix->matrix = realloc(matrix->matrix, sizeof(double *) * matrix->rows);

            *(matrix->matrix + i) = *m->matrix;

            i++;
            node = node->right;
        }

        matrix->matrix[i] = eval_matrix(node, env)->matrix[0];
    }

    return matrix;
}

Matrix* eval_ops(AbstractSyntaxTreeNode *node, Environment *env) {
    if (node->token.state == STATE_NUMBER) {
        Matrix* matrix = matrix_create_new(1, 1);
        sscanf(node->token.value, "%lf", &matrix->matrix[0][0]);

        return matrix;
    } else if (node->token.state == STATE_CHAR) {
        MemoryNode* mem_node = memory_find(env, node->token.value);

        if (mem_node != NULL && mem_node->type == MEM_TYPE_MATRIX) {
            return ((Matrix *) mem_node->value);
        }
    }

    Matrix* m1;
    Matrix* m2;
    Matrix* m;

    switch (node->token.state) {
        case STATE_PLUS:
            if ((node->left->token.state == STATE_NEWLINE_DELIMITER ||
                    node->left->token.state == STATE_DELIMITER) ||
                (node->right->token.state == STATE_NEWLINE_DELIMITER ||
                    node->right->token.state == STATE_DELIMITER)) {
                return matrix_add(eval_matrix(node->left, env), eval_matrix(node->right, env));
            } else {
                return matrix_add(eval_ops(node->left, env), eval_ops(node->right, env));
            }
        case STATE_MINUS:
            if ((node->left->token.state == STATE_NEWLINE_DELIMITER ||
                 node->left->token.state == STATE_DELIMITER) &&
                (node->right->token.state == STATE_NEWLINE_DELIMITER ||
                 node->right->token.state == STATE_DELIMITER)) {
                return matrix_minus(eval_matrix(node->left, env), eval_matrix(node->right, env));
            } else {
                return matrix_minus(eval_ops(node->left, env), eval_ops(node->right, env));
            }
        case STATE_MULTIPLY:
            return matrix_multiply(eval_ops(node->left, env), eval_ops(node->right, env));
        case STATE_DIVIDE:
            m1 = eval_ops(node->left, env);
            m2 = eval_ops(node->right, env);

            if (m1->columns == 1 && m1->rows == 1 && m2->columns == 1 && m2->rows == 1) {
                m = matrix_create_new(1, 1);
                m->matrix[0][0] = m1->matrix[0][0] / m2->matrix[0][0];

                return m;
            }
        default:
            printf("unknown operation %d", node->token.state);
            exit(1);
    }
}

void eval_set_variable(char *name, AbstractSyntaxTreeNode *value, Environment *env) {
    Matrix* m = eval_matrix(value, env);
    memory_set_value(env, name, m, MEM_TYPE_MATRIX);
}