package CBuilder.literals;

import CBuilder.Expression;

import java.util.List;

/**
 * A simple tuple literal.
 */
public class ListLiteral implements Literal {

    /**
     * The list of values the list is containing.
     */
    private final List<Expression> elems;

    /**
     * Create a new list literal with the specified values.
     *
     * @param elems A list of values the list should contain.
     */
    public ListLiteral(List<Expression> elems) {
        this.elems = elems;
    }

    @Override
    public String buildExpression() {
        StringBuilder tupleInit = new StringBuilder();

        for (int i = elems.size() - 1; i >= 0; i--) {
            tupleInit.append("__mpy_list_add(" + elems.get(i).buildExpression() + ", ");
        }
        tupleInit.append("__mpy_obj_init_list()");
        tupleInit.append(")".repeat(elems.size()));

        return tupleInit.toString();
    }

    @Override
    public String buildStatement() {
        return "__mpy_obj_ref_dec(" + buildExpression() + ");\n";
    }

}
