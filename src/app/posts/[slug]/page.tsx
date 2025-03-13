import { getPostBySlug, getRelatedPosts } from '@/lib/blog'
import { Markdown } from '@/components/markdown'
import { BlogPostCard } from '@/components/blog-post-card'
import { notFound } from 'next/navigation'

interface PostPageProps {
  params: {
    slug: string
  }
}

export default function PostPage({ params }: PostPageProps) {
  const post = getPostBySlug(params.slug)
  const relatedPosts = getRelatedPosts(params.slug)

  if (!post) {
    notFound()
  }

  return (
    <article className="min-h-screen bg-black text-gray-100">
      <div className="max-w-4xl mx-auto py-12 px-4">
        <header className="mb-8">
          <div className="flex items-center gap-4 text-sm text-zinc-400 mb-4">
            <time dateTime={post.date}>
              {new Date(post.date).toLocaleDateString('en-US', {
                year: 'numeric',
                month: 'long',
                day: 'numeric',
              })}
            </time>
          </div>
          <h1 className="text-4xl font-bold mb-4">{post.title}</h1>
        </header>

        <div className="prose prose-invert max-w-none">
          <Markdown>{post.content}</Markdown>
        </div>

        {relatedPosts.length > 0 && (
          <section className="mt-16">
            <h2 className="text-2xl font-bold mb-8">Related Posts</h2>
            <div className="grid gap-8 md:grid-cols-2">
              {relatedPosts.map((relatedPost) => (
                <BlogPostCard key={relatedPost.slug} post={relatedPost} />
              ))}
            </div>
          </section>
        )}
      </div>
    </article>
  )
}
