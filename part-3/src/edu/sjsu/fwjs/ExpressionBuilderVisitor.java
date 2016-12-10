package edu.sjsu.fwjs;

import java.util.ArrayList;
import java.util.List;

import edu.sjsu.fwjs.parser.FeatherweightJavaScriptBaseVisitor;
import edu.sjsu.fwjs.parser.FeatherweightJavaScriptParser;

public class ExpressionBuilderVisitor extends FeatherweightJavaScriptBaseVisitor<Expression>{
    @Override
    public Expression visitProg(FeatherweightJavaScriptParser.ProgContext ctx) {
        List<Expression> stmts = new ArrayList<Expression>();
        for (int i=0; i<ctx.stat().size(); i++) {
            Expression exp = visit(ctx.stat(i));
            if (exp != null) stmts.add(exp);
        }
        return listToSeqExp(stmts);
    }

    @Override
    public Expression visitBareExpr(FeatherweightJavaScriptParser.BareExprContext ctx) {
        return visit(ctx.expr());
    }

    @Override
    public Expression visitIfThenElse(FeatherweightJavaScriptParser.IfThenElseContext ctx) {
        Expression cond = visit(ctx.expr());
        Expression thn = visit(ctx.block(0));
        Expression els = visit(ctx.block(1));
        return new IfExpr(cond, thn, els);
    }

    @Override
    public Expression visitIfThen(FeatherweightJavaScriptParser.IfThenContext ctx) {
        Expression cond = visit(ctx.expr());
        Expression thn = visit(ctx.block());
        return new IfExpr(cond, thn, null);
    }

    @Override
    public Expression visitWhile(FeatherweightJavaScriptParser.WhileContext ctx) {
        Expression cond = visit(ctx.expr());
        Expression body = visit(ctx.block());
        return new WhileExpr(cond, body);
    }

    @Override
    public Expression visitPrint(FeatherweightJavaScriptParser.PrintContext ctx) {
        Expression exp = visit(ctx.expr());
        return new PrintExpr(exp);
    }

    @Override
    public Expression visitFunctionApp(FeatherweightJavaScriptParser.FunctionAppContext ctx) {
        Expression exp = visit(ctx.expr());
        List<Expression> args = new ArrayList<Expression>();
        for (int i=1; i<ctx.args().getChildCount()-1; i++) {
            if (i % 2 != 0) {
                Expression aExp = visit(ctx.args().getChild(i));
                args.add(aExp);
            }
        }
        return new FunctionAppExpr(exp, args);
    }

    @Override
    public Expression visitFunctionDec(FeatherweightJavaScriptParser.FunctionDecContext ctx) {
        List<String> params = new ArrayList<String>();
        for (int i=1; i<ctx.params().getChildCount()-1; i++) {
            String param = ctx.params().getChild(i).getText();
            if (!param.equals(",")) {
                params.add(param);
            }
        }
        List<Expression> stmts = new ArrayList<Expression>();
        for (int i=0; i<ctx.stat().size(); i++) {
            Expression exp = visit(ctx.stat(i));
            stmts.add(exp);
        }
        Expression body = listToSeqExp(stmts);
        return new FunctionDeclExpr(params, body);
    }

    @Override
    public Expression visitMulDivMod(FeatherweightJavaScriptParser.MulDivModContext ctx) {
        Expression left = visit(ctx.expr(0));
        Expression right = visit(ctx.expr(1));
        int op = ctx.op.getType();
        if (op == FeatherweightJavaScriptParser.MUL) {
            return new BinOpExpr(Op.MULTIPLY, left, right);
        }
        else if (op == FeatherweightJavaScriptParser.DIV) {
            return new BinOpExpr(Op.DIVIDE, left, right);
        }
        else {
            return new BinOpExpr(Op.MOD, left, right);
        }
    }

    @Override
    public Expression visitAddSub(FeatherweightJavaScriptParser.AddSubContext ctx) {
        Expression left = visit(ctx.expr(0));
        Expression right = visit(ctx.expr(1));
        int op = ctx.op.getType();
        if (op == FeatherweightJavaScriptParser.ADD) {
            return new BinOpExpr(Op.ADD, left, right);
        }
        else {
            return new BinOpExpr(Op.SUBTRACT, left, right);
        }
    }

    @Override
    public Expression visitCompare(FeatherweightJavaScriptParser.CompareContext ctx) {
        Expression left = visit(ctx.expr(0));
        Expression right = visit(ctx.expr(1));
        int op = ctx.op.getType();
        if (op == FeatherweightJavaScriptParser.GT) {
            return new BinOpExpr(Op.GT, left, right);
        }
        else if (op == FeatherweightJavaScriptParser.GE) {
            return new BinOpExpr(Op.GE, left, right);
        }
        else if (op == FeatherweightJavaScriptParser.LT) {
            return new BinOpExpr(Op.LT, left, right);
        }
        else if (op == FeatherweightJavaScriptParser.LE) {
            return new BinOpExpr(Op.LE, left, right);
        }
        else {
            return new BinOpExpr(Op.EQ, left, right);
        }
    }

    @Override
    public Expression visitVarDec(FeatherweightJavaScriptParser.VarDecContext ctx) {
        String id = ctx.ID().getText();
        Expression exp = visit(ctx.expr());
        return new VarDeclExpr(id, exp);
    }

    @Override
    public Expression visitVarRef(FeatherweightJavaScriptParser.VarRefContext ctx) {
        String id = ctx.ID().getText();
        return new VarExpr(id);
    }

    @Override
    public Expression visitVarAssign(FeatherweightJavaScriptParser.VarAssignContext ctx) {
        String id = ctx.ID().getText();
        Expression exp = visit(ctx.expr());
        return new AssignExpr(id, exp);
    }

    @Override
    public Expression visitInt(FeatherweightJavaScriptParser.IntContext ctx) {
        int val = Integer.valueOf(ctx.INT().getText());
        return new ValueExpr(new IntVal(val));
    }

    @Override
    public Expression visitBoolean(FeatherweightJavaScriptParser.BooleanContext ctx) {
        boolean val = Boolean.valueOf(ctx.BOOL().getText());
        return new ValueExpr(new BoolVal(val));
    }

    @Override
    public Expression visitNull(FeatherweightJavaScriptParser.NullContext ctx) {
        return new ValueExpr(new NullVal());
    }

    @Override
    public Expression visitParens(FeatherweightJavaScriptParser.ParensContext ctx) {
        return visit(ctx.expr());
    }

    @Override
    public Expression visitFullBlock(FeatherweightJavaScriptParser.FullBlockContext ctx) {
        List<Expression> stmts = new ArrayList<Expression>();
        for (int i=1; i<ctx.getChildCount()-1; i++) {
            Expression exp = visit(ctx.getChild(i));
            stmts.add(exp);
        }
        return listToSeqExp(stmts);
    }

    /**
     * Converts a list of expressions to one sequence expression,
     * if the list contained more than one expression.
     */
    private Expression listToSeqExp(List<Expression> stmts) {
        if (stmts.isEmpty()) return null;
        Expression exp = stmts.get(0);
        for (int i=1; i<stmts.size(); i++) {
            exp = new SeqExpr(exp, stmts.get(i));
        }
        return exp;
    }

    @Override
    public Expression visitSimpBlock(FeatherweightJavaScriptParser.SimpBlockContext ctx) {
        return visit(ctx.stat());
    }
}
